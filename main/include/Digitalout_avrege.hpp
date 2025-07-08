#ifndef __DIGITAL_OUTPUTAVREGE_HPP__
#define __DIGITAL_OUTPUTAVREGE_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

namespace cadmium
{

    // State structure for the digital output model
    struct DigitalOutputavregeState
    {
        bool output;  // Current output value (true = HIGH, false = LOW)
        double sigma; // Time until next internal transition (not used here)

        explicit DigitalOutputavregeState() : output(false), sigma(0) {}
    };

    // Optional: logging operator for the state (currently empty)
    std::ostream &operator<<(std::ostream &out, const DigitalOutputavregeState &state)
    {
        return out;
    }

    /**
     * DigitalOutputavrege: A DEVS atomic model that writes a boolean signal to a GPIO pin.
     * It listens to a boolean input and sets the corresponding hardware pin high or low.
     */
    class DigitalOutputavrege : public Atomic<DigitalOutputavregeState>
    {
    public:
        Port<bool> in; // Input port receiving a boolean signal

        // STM32 hardware configuration
        GPIO_TypeDef *port;    // GPIO port (e.g., GPIOA, GPIOB)
        GPIO_InitTypeDef pins; // GPIO pin configuration

        /**
         * Constructor
         * @param id - unique model identifier
         * @param selectedPort - GPIO port (e.g., GPIOA, GPIOB)
         * @param selectedPins - GPIO configuration structure
         */
        DigitalOutputavrege(const std::string &id, GPIO_TypeDef *selectedPort, GPIO_InitTypeDef *selectedPins)
            : Atomic<DigitalOutputavregeState>(id, DigitalOutputavregeState()), port(selectedPort), pins(*selectedPins)
        {
            in = addInPort<bool>("in");

            // Enable GPIO clocks (hardcoded for GPIOA and GPIOB)
            __HAL_RCC_GPIOB_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();

            // Initialize the GPIO pin
            HAL_GPIO_Init(port, &pins);
        }

        /**
         * Internal transition: no internal state change needed.
         */
        void internalTransition(DigitalOutputavregeState &state) const override
        {
            (void)state;
        }

        /**
         * External transition: reacts to input message by updating output state and writing to GPIO pin.
         */
        void externalTransition(DigitalOutputavregeState &state, double /*e*/) const override
        {
            if (!in->empty())
            {
                for (const auto value : in->getBag())
                {
                    state.output = value;
                }
                // Set or reset the pin depending on the boolean value
                HAL_GPIO_WritePin(port, pins.Pin, state.output ? GPIO_PIN_SET : GPIO_PIN_RESET);
            }
        }

        /**
         * Output function: no messages are sent to other models
         */
        void output(const DigitalOutputavregeState &state) const override
        {
            (void)state; // No output message
        }

        /**
         * Time advance function: since no internal events, wait indefinitely
         */
        [[nodiscard]] double timeAdvance(const DigitalOutputavregeState & /*state*/) const override
        {
            return std::numeric_limits<double>::infinity();
        }
    };

} // namespace cadmium

#endif // __DIGITAL_OUTPUTAVREGE_HPP__
