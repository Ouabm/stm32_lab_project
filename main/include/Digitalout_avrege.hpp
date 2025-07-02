#ifndef __DIGITAL_OUTPUTAVREGE_HPP__
#define __DIGITAL_OUTPUTAVREGE_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h" 
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

namespace cadmium {

    struct DigitalOutputavregeState {
        bool output;
        double sigma;
       explicit DigitalOutputavregeState() : output(false),sigma(0) {}
    };


    std::ostream& operator<<(std::ostream &out, const DigitalOutputavregeState& state) {
       
        return out;
    }


    class DigitalOutputavrege : public Atomic<DigitalOutputavregeState> {
    public:
        
        Port <bool> in;
       

        // Hardware references
        GPIO_TypeDef* port;
        GPIO_InitTypeDef pins;

        DigitalOutputavrege(const std::string& id, GPIO_TypeDef* selectedPort, GPIO_InitTypeDef* selectedPins)
        : Atomic<DigitalOutputavregeState>(id, DigitalOutputavregeState()), port(selectedPort), pins(*selectedPins)
        {
            in =addInPort <bool> ("in");
            
            
            __HAL_RCC_GPIOB_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();
            
            HAL_GPIO_Init(port, &pins);
        }

        void internalTransition(DigitalOutputavregeState& state) const override {
            (void)state;
             
        }

        void externalTransition(DigitalOutputavregeState& state, double /*e*/) const override {
            if (!in->empty()) {
                for (const auto value : in->getBag()) {
                    state.output = value;
                }
               HAL_GPIO_WritePin(port, pins.Pin, state.output ? GPIO_PIN_SET : GPIO_PIN_RESET);// Nothing internally changes on its own
            }
        }

        void output(const DigitalOutputavregeState& state) const override {
            (void)state;
           
    }

        [[nodiscard]] double timeAdvance(const DigitalOutputavregeState& /*state*/) const override {
            return std::numeric_limits<double>::infinity();
        }
    };

} // namespace cadmium

#endif // __DIGITAL_OUTPUT_HPP__
