#ifndef __DIGITAL_OUTPUTBAD_HPP__
#define __DIGITAL_OUTPUTBAD_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h" 
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

namespace cadmium {

    struct DigitalOutputbadState {
        bool output;
        double sigma;
       explicit DigitalOutputbadState() : output(false),sigma(0) {}
    };


    std::ostream& operator<<(std::ostream &out, const DigitalOutputbadState& state) {
     
        return out;
    }


    class DigitalOutputbad : public Atomic<DigitalOutputbadState> {
    public:
        
        Port <bool> in;
       

        // Hardware references
        GPIO_TypeDef* port;
        GPIO_InitTypeDef pins;

        DigitalOutputbad(const std::string& id, GPIO_TypeDef* selectedPort, GPIO_InitTypeDef* selectedPins)
        : Atomic<DigitalOutputbadState>(id, DigitalOutputbadState()), port(selectedPort), pins(*selectedPins)
        {
            in =addInPort <bool> ("in");
           
            
            __HAL_RCC_GPIOB_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();
            
            HAL_GPIO_Init(port, &pins);
        }

        void internalTransition(DigitalOutputbadState& state) const override {
            (void)state;
             
        }

        void externalTransition(DigitalOutputbadState& state, double /*e*/) const override {
            if (!in->empty()) {
                for (const auto value : in->getBag()) {
                    state.output = value;
                }
               HAL_GPIO_WritePin(port, pins.Pin, state.output ? GPIO_PIN_SET : GPIO_PIN_RESET);// Nothing internally changes on its own
            }
        }

        void output(const DigitalOutputbadState& state) const override {
            (void)state;
           
    }

        [[nodiscard]] double timeAdvance(const DigitalOutputbadState& /*state*/) const override {
            return std::numeric_limits<double>::infinity();
        }
    };

} // namespace cadmium

#endif // __DIGITAL_OUTPUT_HPP__
