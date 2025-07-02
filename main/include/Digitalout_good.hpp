#ifndef __DIGITAL_OUTPUTGOOD_HPP__
#define __DIGITAL_OUTPUTGOOD_HPP__

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_gpio.h" 
#include "stm32h7xx_hal_rcc.h"
#include "stm32h743xx.h"

namespace cadmium {

    struct DigitalOutputgoodState {
        bool output;
        double sigma;
       explicit DigitalOutputgoodState() : output(false),sigma(0) {}
    };


    std::ostream& operator<<(std::ostream &out, const DigitalOutputgoodState& state) {
        
        return out;
    }


    class DigitalOutputgood : public Atomic<DigitalOutputgoodState> {
    public:
        
        Port <bool> in;
       

        // Hardware references
        GPIO_TypeDef* port;
        GPIO_InitTypeDef pins;

        DigitalOutputgood(const std::string& id, GPIO_TypeDef* selectedPort, GPIO_InitTypeDef* selectedPins)
        : Atomic<DigitalOutputgoodState>(id, DigitalOutputgoodState()), port(selectedPort), pins(*selectedPins)
        {
            in =addInPort <bool> ("in");
            
            
            __HAL_RCC_GPIOB_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();
            
            HAL_GPIO_Init(port, &pins);
        }

        void internalTransition(DigitalOutputgoodState& state) const override {
            (void)state;
             
        }

        void externalTransition(DigitalOutputgoodState& state, double /*e*/) const override {
            if (!in->empty()) {
                for (const auto value : in->getBag()) {
                    state.output = value;
                }
               HAL_GPIO_WritePin(port, pins.Pin, state.output ? GPIO_PIN_SET : GPIO_PIN_RESET);// Nothing internally changes on its own
            }
        }

        void output(const DigitalOutputgoodState& state) const override {
            (void)state;
           
    }

        [[nodiscard]] double timeAdvance(const DigitalOutputgoodState& /*state*/) const override {
            return std::numeric_limits<double>::infinity();
        }
    };

} // namespace cadmium

#endif // __DIGITAL_OUTPUT_HPP__
