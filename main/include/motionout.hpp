#ifndef __DIGITAL_MOTION_HPP__
#define __DIGITAL_MOTION_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h" 
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

namespace cadmium {

    struct DigitalOutputState {
        bool output;
        double sigma;
       explicit DigitalOutputState() : output(true),sigma(0) {}
    };


    std::ostream& operator<<(std::ostream &out, const DigitalOutputState& state) {
       
        return out;
    }


    class DigitalOutput : public Atomic<DigitalOutputState> {
    public:
        
        Port <bool> in;
       

        // Hardware references
        GPIO_TypeDef* port;
        GPIO_InitTypeDef pins;

        DigitalOutput(const std::string& id, GPIO_TypeDef* selectedPort, GPIO_InitTypeDef* selectedPins)
        : Atomic<DigitalOutputState>(id, DigitalOutputState()), port(selectedPort), pins(*selectedPins)
        {
            in =addInPort <bool> ("in");
            
            
           
            
            HAL_GPIO_Init(port, &pins);
        }

        void internalTransition(DigitalOutputState& state) const override {
            (void)state;
             
        }

        void externalTransition(DigitalOutputState& state, double /*e*/) const override {
            if (!in->empty()) {
                for (const auto value : in->getBag()) {
                    state.output = value;
                }
               HAL_GPIO_WritePin(port, pins.Pin, state.output ? GPIO_PIN_SET : GPIO_PIN_RESET);// Nothing internally changes on its own
            }
        }

        void output(const DigitalOutputState& state) const override {
            (void)state;
           
    }

        [[nodiscard]] double timeAdvance(const DigitalOutputState& /*state*/) const override {
            return std::numeric_limits<double>::infinity();
        }
    };

} // namespace cadmium

#endif // __DIGITAL_OUTPUT_HPP__
