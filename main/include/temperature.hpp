#ifndef RT_TEMPERATURESENSORINPUT_HPP
#define RT_TEMPERATURESENSORINPUT_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal.h"

extern "C"
{
#include "DHT_11/DHT.h"
#include "tim.h"
}

#ifndef NO_LOGGING
#include <iostream>
#include <fstream>
#include <string>
#endif

using namespace std;

namespace cadmium
{

    // State structure for the temperature sensor input model
    struct TemperatureSensorInputState
    {
        bool output;           // Output boolean indicating if temperature is above threshold
        double sigma;          // Time until next internal transition
        float Temperature;     // Current temperature value read from sensor
        float lastTemperature; // Last temperature value (not used currently)

        TemperatureSensorInputState() : output(false), sigma(0.0), Temperature(100) {}
    };

    // Stream operator for debug/logging: outputs the current temperature
    inline std::ostream &operator<<(std::ostream &out, const TemperatureSensorInputState &state)
    {
        out << "Temperature: " << state.Temperature;
        return out;
    }

    /**
     * TemperatureSensorInput: DEVS atomic model for reading temperature from a DHT11 sensor.
     * It periodically reads sensor data, validates checksum, updates temperature,
     * and outputs a boolean signal indicating if temperature is above 25°C.
     */
    class TemperatureSensorInput : public Atomic<TemperatureSensorInputState>
    {
    public:
        Port<bool> out; // Output port sending true if temperature > 25°C, else false

        TemperatureSensorInput(const std::string &id)
            : Atomic<TemperatureSensorInputState>(id, TemperatureSensorInputState())
        {
            out = addOutPort<bool>("out");
        }

        /**
         * Internal transition triggered periodically:
         * Reads temperature from DHT11 sensor, checks data integrity,
         * updates temperature and output status.
         */
        void internalTransition(TemperatureSensorInputState &state) const override
        {
            uint8_t RHI, RHD, TCI, TCD, SUM;
            float tCelsius = 0.0f;

            // Start communication with DHT11 sensor
            if (DHT11_Start())
            {
                // Read humidity integer and decimal parts (unused here but read for checksum)
                RHI = DHT11_Read();
                RHD = DHT11_Read();
                // Read temperature integer and decimal parts
                TCI = DHT11_Read();
                TCD = DHT11_Read();
                // Read checksum
                SUM = DHT11_Read();

                // Verify checksum to confirm data validity
                if ((uint8_t)(RHI + RHD + TCI + TCD) == SUM)
                {
                    // Calculate temperature in Celsius
                    tCelsius = TCI + (TCD / 10.0f);
                    state.Temperature = tCelsius;
                }
                else
                {
                    // Checksum error: assign error value
                    state.Temperature = 100;
                }
            }
            else
            {
                // Sensor start failure: assign error value
                state.Temperature = 100;
            }

            // Update output boolean: true if temperature > 25°C, false otherwise
            state.output = (state.Temperature > 25);

            // Set next transition time to 2 seconds (polling interval)
            state.sigma = 2.0;
        }

        /**
         * External transition: no external inputs handled here.
         */
        void externalTransition(TemperatureSensorInputState &state, double e) const override
        {
            (void)state;
            (void)e;
        }

        /**
         * Output function: sends output boolean on the out port.
         */
        void output(const TemperatureSensorInputState &state) const override
        {
            out->addMessage(state.output);
        }

        /**
         * Time advance: returns time until next internal event (polling interval).
         */
        [[nodiscard]] double timeAdvance(const TemperatureSensorInputState &state) const override
        {
            return state.sigma;
        }
    };

} // namespace cadmium

#endif // RT_TEMPERATURESENSORINPUT_HPP
