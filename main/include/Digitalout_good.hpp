#ifndef __DIGITAL_OUTPUTGOOD_HPP__
#define __DIGITAL_OUTPUTGOOD_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h" 
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

namespace cadmium {

    // Structure representing the state of the digital output model for "good" CO₂ level
    struct DigitalOutputgoodState {
        bool output;     // Current output value (true = HIGH, false = LOW)
        double sigma;    // Time until next internal transition (unused here)

        explicit DigitalOutputgoodState() : output(false), sigma(0) {}
    };

    // Optional: output stream operator for debugging (currently empty)
    std::ostream& operator<<(std::ostream &out, const DigitalOutputgoodState& state) {
        return out;
    }

    /**
     * DigitalOutputgood: A DEVS atomic model that sets a GPIO pin state according to
     * the input boolean value. Used to indicate a "good" CO₂ condition.
     */
    class DigitalOutputgood : public Atomic<DigitalOutputgoodState> {
    public:

        Port<bool> in;  // Input port receiving boolean signal to write to the pin

        // Hardware configuration references
        GPIO_TypeDef* port;           // GPIO port (e.g., GPIOA, GPIOB)
        GPIO_InitTypeDef pins;        // GPIO pin configuration

        /**
         * Constructor
         * @param id - Unique model identifier
         * @param selectedPort - GPIO port for output
         * @param selectedPins - GPIO pin configuration
         */
        DigitalOutputgood(const std::string& id, GPIO_TypeDef* selectedPort, GPIO_InitTypeDef* selectedPins)
        : Atomic<DigitalOutputgoodState>(id, DigitalOutputgoodState()), port(selectedPort), pins(*selectedPins)
        {
            in = addInPort<bool>("in");

            // Enable clocks for GPIO ports (hardcoded for GPIOA and GPIOB)
            __HAL_RCC_GPIOB_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();

            // Initialize the GPIO pin with HAL library
            HAL_GPIO_Init(port, &pins);
        }

        /**
         * Internal transition: no internal state changes, remains passive
         */
        void internalTransition(DigitalOutputgoodState& state) const override {
            (void)state;
        }

        /**
         * External transition: triggered by new input on port 'in'.
         * Updates the output state and writes to the hardware pin accordingly.
         */
        void externalTransition(DigitalOutputgoodState& state, double /*e*/) const override {
            if (!in->empty()) {
                for (const auto value : in->getBag()) {
                    state.output = value;
                }
                // Write to GPIO pin: set or reset depending on the input boolean value
                HAL_GPIO_WritePin(port, pins.Pin, state.output ? GPIO_PIN_SET : GPIO_PIN_RESET);
            }
        }

        /**
         * Output function: no output messages to other models
         */
        void output(const DigitalOutputgoodState& state) const override {
            (void)state;
        }

        /**
         * Time advance function: no internal events, wait indefinitely
         */
        [[nodiscard]] double timeAdvance(const DigitalOutputgoodState& /*state*/) const override {
            return std::numeric_limits<double>::infinity();
        }
    };

} // namespace cadmium

#endif // __DIGITAL_OUTPUTGOOD_HPP__
