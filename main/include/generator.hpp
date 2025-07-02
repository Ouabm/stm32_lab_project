#ifndef SERVO_COMMAND_GENERATOR_HPP
#define SERVO_COMMAND_GENERATOR_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <limits>
#include <iostream>
#include <vector>

namespace cadmium {

    struct ServoCommandState {
        bool input;
        double speed;
        double sigma;
        size_t index;
        bool send_command;
        std::vector<double> commands;

        ServoCommandState() : input(false), speed(0.0), sigma(std::numeric_limits<double>::infinity()),
                              index(0), send_command(false),
                              commands({0.25, 0.5, 0.75}) {}
    };

    std::ostream& operator<<(std::ostream &out, const ServoCommandState& state) {
        
        return out;
    }

    class ServoCommandGenerator : public Atomic<ServoCommandState> {
    public:
        Port<double> out;
        Port<bool> in;

        ServoCommandGenerator(const std::string& id)
            : Atomic<ServoCommandState>(id, ServoCommandState()) {
            out = addOutPort<double>("out");
            in = addInPort<bool>("in");
        }

        void internalTransition(ServoCommandState& state) const override {
            state.send_command = false;  // Reset le flag d'émission

            if (state.input && state.index + 1 < state.commands.size()) {
                state.index++;
                state.speed = state.commands[state.index];
                state.sigma = 1.0;
            }
        }

        void externalTransition(ServoCommandState& state, double e) const override {
            if (!in->empty()) {
                for (const auto value : in->getBag()) {
                    state.input = value;
                }

                if (state.input) {
                    state.send_command = true;
                    state.speed = state.commands[state.index];
                    state.sigma=1.0;
                    
                } else {
                    state.speed=-0.5;
                    state.sigma=1.0;
                }
              
            }
            
        }

        void output(const ServoCommandState& state) const override {
                out->addMessage(state.speed);
            
        }

        [[nodiscard]] double timeAdvance(const ServoCommandState& state) const override {
            return state.sigma;
        }
    };
}

#endif // SERVO_COMMAND_GENERATOR_HPP
