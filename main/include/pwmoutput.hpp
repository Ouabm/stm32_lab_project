#ifndef RT_PWMOUTPUT_HPP
#define RT_PWMOUTPUT_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <limits>
#include <iostream>
#include <algorithm>
#include "stm32h7xx_hal.h"  // à adapter selon ta carte (stm32f1xx_hal.h, etc.)

using namespace std;

namespace cadmium {

    struct PWMOutputState {
        double output;
        double sigma;

        explicit PWMOutputState() : output(0.0), sigma(1.0) {}
    };

    std::ostream& operator<<(std::ostream &out, const PWMOutputState& state) {
        out << "PWM Output Duty Cycle: " << state.output;
        return out;
    }

    class PWMOutput : public Atomic<PWMOutputState> {
    public:
        Port<double> in;
        TIM_HandleTypeDef* timer;  // Pointeur vers un timer initialisé (ex: &htim3)
        uint32_t channel;          // Canal (ex: TIM_CHANNEL_1)
        uint32_t period_ticks;     // Nombre de ticks de la période du timer

        PWMOutput(const std::string& id, TIM_HandleTypeDef* t, uint32_t ch, uint32_t period) 
            : Atomic<PWMOutputState>(id, PWMOutputState()), timer(t), channel(ch), period_ticks(period) 
        {
            in = addInPort<double>("in");

            // Assumer que HAL_TIM_PWM_Start() a déjà été appelé dans main.c
            setPWMDutyCycle(0.0); // duty cycle à 0% au début
        }

        void internalTransition(PWMOutputState& state) const override {
            // Aucun comportement interne
        }

        void externalTransition(PWMOutputState& state, double e) const override {
            if (!in->empty()) {
                for (const auto& x : in->getBag()) {
                    state.output = std::clamp(x, 0.0, 1.0);
                }
                setPWMDutyCycle(state.output);
            }
        }

        void output(const PWMOutputState& state) const override {
            // Pas d'output DEVS ici
        }

        [[nodiscard]] double timeAdvance(const PWMOutputState& state) const override {
            return std::numeric_limits<double>::infinity();
        }

    private:
        void setPWMDutyCycle(double duty) const {
            uint32_t compare = static_cast<uint32_t>(duty * period_ticks);
            __HAL_TIM_SET_COMPARE(timer, channel, compare);//mettre compare dans dutycycle dans le bon registre 
        }
    };
}

#endif // RT_PWMOUTPUT_HPP
