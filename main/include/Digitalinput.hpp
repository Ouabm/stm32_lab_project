/**
 * ARSLab - Carleton University
 * 
 * A DEVS model for digital input pins on the STM32H7 boards (adapted from MSP432 version).
 * This model polls a GPIO pin periodically and outputs its logical state as a boolean.
 * It can be used to model buttons or any digital sensor that provides a binary signal.
 */

#ifndef __DIGITAL_INPUT_HPP__
#define __DIGITAL_INPUT_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h" 
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

#ifndef NO_LOGGING
    #include <iostream>
#endif

namespace cadmium {

    // State of the digital input model
    struct DigitalInputState {
        bool output;   // Current logic level of the input pin
        bool last;     // Previous logic level (optional for change detection)
        double sigma;  // Time until next internal transition

        // Constructor initializing default values
        explicit DigitalInputState() : output(false), last(false), sigma(0.1) {}
    };

#ifndef NO_LOGGING
    /**
     * Overload for << operator to log the pin value
     */
    std::ostream& operator<<(std::ostream &out, const DigitalInputState& state) {
        out << "Pin: " << (state.output ? 1 : 0); 
        return out;
    }
#endif

    // Atomic DEVS model that reads a digital input pin periodically
    class DigitalInput : public Atomic<DigitalInputState> {
    public:
        Port<bool> out; // Output port: sends the logic level of the pin

        // STM32 hardware configuration
        GPIO_TypeDef* port;            // GPIO port (e.g., GPIOA, GPIOB)
        GPIO_InitTypeDef pins;         // Pin configuration structure
        uint16_t pinNumber;            // Specific pin number (e.g., GPIO_PIN_5)

        /**
         * Constructor
         * @param id Unique ID of the DEVS model
         * @param selectedPort GPIO port to read from
         * @param selectedPins Pointer to pin configuration
         */
        DigitalInput(const std::string& id, GPIO_TypeDef* selectedPort, GPIO_InitTypeDef* selectedPins)
            : Atomic<DigitalInputState>(id, DigitalInputState()), port(selectedPort), pins(*selectedPins)
        {
            out = addOutPort<bool>("out");
            HAL_GPIO_Init(port, &pins); // Initialize the pin with HAL
        };

        /**
         * Internal transition function: reads the pin state
         */
        void internalTransition(DigitalInputState& state) const override {
            GPIO_PinState pinstate = HAL_GPIO_ReadPin(port, pins.Pin);
            state.output = (pinstate == GPIO_PIN_SET);
            state.sigma = 0.5; // Poll every 0.5 seconds
        }

        /**
         * External transition should not be triggered, as the model has no input port
         */
        void externalTransition(DigitalInputState& state, double e) const override {
            throw CadmiumSimulationException("External transition called in a model with no input ports");
        }

        /**
         * Output function: sends current pin state to the output port
         */
        void output(const DigitalInputState& state) const override {
            out->addMessage(state.output);
        }

        /**
         * Time advance function: returns time until next polling
         */
        [[nodiscard]] double timeAdvance(const DigitalInputState& state) const override {
            return state.sigma;
        }
    };
} // namespace cadmium

#endif // __DIGITAL_INPUT_HPP__
