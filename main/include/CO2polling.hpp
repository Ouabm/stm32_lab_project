#ifndef RT_ANALOGINPUT_HPP
#define RT_ANALOGINPUT_HPP

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_dac.h"
extern "C"
{
#include "adc.h"
}

using namespace std;

namespace cadmium
{

    // State structure for the AnalogInput model
    struct AnalogInputState
    {
        float output;           // Output value in ppm
        double sigma;           // Time until next internal transition
        float tab[21] = {0.0f}; // Circular buffer for smoothing
        int index = 0;          // Current index in the buffer
        AnalogInputState() : output(0.0), sigma(1.0) {}
    };

    // Logging state information to the output stream
    std::ostream &operator<<(std::ostream &out, const AnalogInputState &state)
    {
        out << "Analog value: " << state.output;
        return out;
    }

    // AnalogInput atomic model class
    class AnalogInput : public Atomic<AnalogInputState>
    {
    public:
        Port<float> out; // Output port

        // Constructor: initializes the model with GPIO and ADC handles
        AnalogInput(const std::string &id, GPIO_TypeDef *selectedPort, ADC_HandleTypeDef *pin)
            : Atomic<AnalogInputState>(id, AnalogInputState()), port(selectedPort), analogPin(pin), pollingRate(1.0)
        {
            out = addOutPort<float>("out");
        }

        GPIO_TypeDef *port;           // GPIO port (not used in logic, but kept for completeness)
        ADC_HandleTypeDef *analogPin; // Pointer to ADC peripheral
        double pollingRate;           // Time interval between ADC readings (not currently used)

        // Internal transition: read analog value, convert to voltage, compute ppm
        void internalTransition(AnalogInputState &state) const override
        {
            // Start ADC conversion
            HAL_ADC_Start(analogPin);
            HAL_ADC_PollForConversion(analogPin, 20);   // Wait for conversion to complete (timeout = 20ms)
            uint16_t raw = HAL_ADC_GetValue(analogPin); // Get raw ADC value

            // Convert raw value to voltage (assuming 10-bit ADC and 5V reference)
            float voltage = (raw / 1024.0f) * 5.0f;

            // Adjust based on sensor output ratio (e.g., voltage divider or amplifier gain)
            float Vout = voltage / 8.5f;

            // Store value in circular buffer
            state.tab[state.index] = Vout;
            state.index = (state.index + 1) % 21;

            // Compute average value (Vref) for smoothing
            float sum = 0.0f;
            for (int i = 0; i < 21; i++)
            {
                sum += state.tab[i];
            }
            float Vref = sum / 21;

            // Convert voltage to CO₂ ppm using sensor's response curve
            float ppm = 0.0f;
            float slope = 0.030f / (2.602f - 3.0f);              // Example calibration slope
            ppm = powf(10.0f, ((Vout - Vref) / slope + 2.602f)); // Logarithmic formula

            // Update state
            state.output = ppm;
            state.sigma = 0.8; // Wait 0.8s before next reading
        }

        // External transition: not used (no input port in this model)
        void externalTransition(AnalogInputState &state, double e) const override
        {
            // No input to process, model is purely time-driven
        }

        // Output function: send current ppm value through output port
        void output(const AnalogInputState &state) const override
        {
            out->addMessage(state.output);
        }

        // Time advance function: returns how long to wait before next internal transition
        [[nodiscard]] double timeAdvance(const AnalogInputState &state) const override
        {
            return state.sigma;
        }
    };

} // namespace cadmium

#endif // RT_ANALOGINPUT_HPP
