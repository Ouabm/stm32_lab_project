#ifndef __DIGITAL_MOTION_HPP__
#define __DIGITAL_MOTION_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

namespace cadmium
{

    // Structure representing the state of the digital output model for motion detection
    struct DigitalOutputState
    {
        bool output;  // Current output value (true = HIGH, false = LOW)
        double sigma; // Time until next internal transition (unused here)

        explicit DigitalOutputState() : output(true), sigma(0) {}
    };

    // Optional stream operator for debugging (empty here)
    std::ostream &operator<<(std::ostream &out, const DigitalOutputState &state)
    {
        return out;
    }

    /**
     * DigitalOutput: A DEVS atomic model that sets a GPIO pin based on
     * input boolean values. Used here likely to represent motion sensor output.
     */
    class DigitalOutput : public Atomic<DigitalOutputState>
    {
    public:
        Port<bool> in; // Input port to receive boolean signal (e.g., motion detected or not)

        // Hardware references for STM32 GPIO
        GPIO_TypeDef *port;    // GPIO port (e.g., GPIOA, GPIOB)
        GPIO_InitTypeDef pins; // GPIO pin configuration

        /**
         * Constructor
         * @param id - Unique model identifier
         * @param selectedPort - GPIO port where the output pin is connected
         * @param selectedPins - GPIO pin initialization parameters
         */
        DigitalOutput(const std::string &id, GPIO_TypeDef *selectedPort, GPIO_InitTypeDef *selectedPins)
            : Atomic<DigitalOutputState>(id, DigitalOutputState()), port(selectedPort), pins(*selectedPins)
        {
            in = addInPort<bool>("in");

            // Initialize GPIO pin with STM32 HAL
            HAL_GPIO_Init(port, &pins);
        }

        /**
         * Internal transition: no internal behavior, passive model
         */
        void internalTransition(DigitalOutputState &state) const override
        {
            (void)state;
        }

        /**
         * External transition: triggered by new input.
         * Updates internal state and sets the GPIO pin accordingly.
         */
        void externalTransition(DigitalOutputState &state, double /*e*/) const override
        {
            if (!in->empty())
            {
                for (const auto value : in->getBag())
                {
                    state.output = value;
                }
                // Write output value to GPIO pin: set or reset
                HAL_GPIO_WritePin(port, pins.Pin, state.output ? GPIO_PIN_RESET : GPIO_PIN_SET);
            }
        }

        /**
         * Output function: no messages are sent to other models
         */
        void output(const DigitalOutputState &state) const override
        {
            (void)state;
        }

        /**
         * Time advance: wait indefinitely for external events
         */
        [[nodiscard]] double timeAdvance(const DigitalOutputState & /*state*/) const override
        {
            return std::numeric_limits<double>::infinity();
        }
    };

} // namespace cadmium

#endif // __DIGITAL_MOTION_HPP__
