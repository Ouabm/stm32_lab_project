#ifndef __DIGITAL_OUTPUTBAD_HPP__
#define __DIGITAL_OUTPUTBAD_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

namespace cadmium
{

    // Structure representing the state of the DigitalOutputbad model
    struct DigitalOutputbadState
    {
        bool output;  // Current boolean value to be written to the pin
        double sigma; // Time until next internal transition (not used here)

        explicit DigitalOutputbadState() : output(false), sigma(0) {}
    };

    // Optional: output stream operator for state (unused in logging here)
    std::ostream &operator<<(std::ostream &out, const DigitalOutputbadState &state)
    {
        return out;
    }

    /**
     * DigitalOutputbad: A DEVS atomic model that writes a boolean value to a GPIO pin
     * when a message is received on the input port. It represents the “bad” CO₂ level.
     */
    class DigitalOutputbad : public Atomic<DigitalOutputbadState>
    {
    public:
        Port<bool> in; // Input port receiving the boolean signal

        // Hardware references for STM32 GPIO
        GPIO_TypeDef *port;    // Pointer to GPIO port (e.g., GPIOA, GPIOB)
        GPIO_InitTypeDef pins; // GPIO pin configuration structure

        /**
         * Constructor
         * @param id - Model ID
         * @param selectedPort - GPIO port where the signal should be written
         * @param selectedPins - GPIO pin configuration
         */
        DigitalOutputbad(const std::string &id, GPIO_TypeDef *selectedPort, GPIO_InitTypeDef *selectedPins)
            : Atomic<DigitalOutputbadState>(id, DigitalOutputbadState()), port(selectedPort), pins(*selectedPins)
        {
            in = addInPort<bool>("in");

            // Initialize the GPIO pin
            HAL_GPIO_Init(port, &pins);
        }

        /**
         * Internal transition: no internal behavior to process, stays passive
         */
        void internalTransition(DigitalOutputbadState &state) const override
        {
            (void)state;
        }

        /**
         * External transition: triggered by a new input message.
         * Updates output state and writes to the specified pin.
         */
        void externalTransition(DigitalOutputbadState &state, double /*e*/) const override
        {
            if (!in->empty())
            {
                for (const auto value : in->getBag())
                {
                    state.output = value;
                }
                // Write the value to the hardware pin
                HAL_GPIO_WritePin(port, pins.Pin, state.output ? GPIO_PIN_SET : GPIO_PIN_RESET);
            }
        }

        /**
         * Output function: no data sent to other models, only hardware interaction
         */
        void output(const DigitalOutputbadState &state) const override
        {
            (void)state;
        }

        /**
         * Time advance function: model remains passive indefinitely
         */
        [[nodiscard]] double timeAdvance(const DigitalOutputbadState & /*state*/) const override
        {
            return std::numeric_limits<double>::infinity();
        }
    };

} // namespace cadmium

#endif // __DIGITAL_OUTPUTBAD_HPP__
