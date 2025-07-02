/**
 * Srijan Gupta & James Grieder
 * ARSLab - Carleton University
 * 
 * A DEVS model for digital input pins on the MSP432P401R board.  This model can be 
 * used for switch inputs on the MSP432P401R, and switch and gator hole inputs on 
 * the Educational Boosterpack MK II.  
 * 
 * The specified pins are polled every 0.1 seconds, and will output the pin status 
 * as a boolean.
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
    struct DigitalInputState {
        bool output;
        bool last;
        double sigma;

        /**
        * DigitalInputState constructor.
        */
        explicit DigitalInputState(): output(true), last(false), sigma(0.1){}
    };

#ifndef NO_LOGGING
    /**
     * Insertion operator for DigitalInputState objects.
     * @param out output stream.
     * @param s state to be represented in the output stream.
     * @return output stream.
     */
    std::ostream& operator<<(std::ostream &out, const DigitalInputState& state) {
        out << "Pin: " << (state.output ? 1 : 0); 
        return out;
    }
#endif
    class DigitalInput : public Atomic<DigitalInputState> {
      public:
      
        Port<bool> out;

        //Parameters to be overwritten when instantiating the atomic model
        GPIO_TypeDef* port;
        GPIO_InitTypeDef pins;
        uint16_t pinNumber;

        // default constructor
        DigitalInput(const std::string& id, GPIO_TypeDef* selectedPort, GPIO_InitTypeDef* selectedPins): Atomic<DigitalInputState>(id, DigitalInputState()), port(selectedPort), pins(*selectedPins)  {
            out = addOutPort<bool>("out");
             __HAL_RCC_GPIOC_CLK_ENABLE();
            HAL_GPIO_Init(port,&pins);

            
            state.last = state.output;
            
        };
      
        // internal transition
        void internalTransition(DigitalInputState& state) const override {
            GPIO_PinState pinstate;
            bool bouton;
            state.last = state.output;
            pinstate = HAL_GPIO_ReadPin(port,pins.Pin);
            if(pinstate==GPIO_PIN_SET){
               bouton = false;
            }
            else{
                bouton = true;
            }
            state.output=bouton;
           
            state.sigma = 0.1;
        }

        // external transition
        void externalTransition(DigitalInputState& state, double e) const override {
          throw CadmiumSimulationException("External transition called in a model with no input ports");
        }
      
        // output function
        void output(const DigitalInputState& state) const override {
            if(state.last != state.output) {
                bool output=state.output;
                out->addMessage(state.output);
            }
        }

        // time_advance function
        [[nodiscard]] double timeAdvance(const DigitalInputState& state) const override {
            return state.sigma;
        }
    };
} // namespace cadmium

#endif // __DIGITAL_INPUT_HPP__