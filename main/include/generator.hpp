#ifndef SERVO_COMMAND_GENERATOR_HPP
#define SERVO_COMMAND_GENERATOR_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <limits>
#include <iostream>
#include <vector>

namespace cadmium
{

    // State structure for the ServoCommandGenerator atomic model
    struct ServoCommandState
    {
        bool input;                   // Input signal indicating whether to send commands
        double speed;                 // Current speed command to output
        double sigma;                 // Time until next internal transition
        size_t index;                 // Index to track current command in the sequence
        bool send_command;            // Flag to indicate if a command should be sent
        std::vector<double> commands; // Predefined speed commands

        ServoCommandState()
            : input(false),
              speed(0.0),
              sigma(std::numeric_limits<double>::infinity()),
              index(0),
              send_command(false),
              commands({0.25, 0.5, 0.75}) {}
    };

    // Optional: output stream operator for debugging (empty implementation)
    std::ostream &operator<<(std::ostream &out, const ServoCommandState &state)
    {
        return out;
    }

    /**
     * ServoCommandGenerator: A DEVS atomic model generating servo speed commands
     * based on input boolean signals. Commands are issued sequentially from a predefined list.
     */
    class ServoCommandGenerator : public Atomic<ServoCommandState>
    {
    public:
        Port<double> out; // Output port for speed commands
        Port<bool> in;    // Input port to receive trigger signals

        /**
         * Constructor
         * @param id Unique identifier for the atomic model
         */
        ServoCommandGenerator(const std::string &id)
            : Atomic<ServoCommandState>(id, ServoCommandState())
        {
            out = addOutPort<double>("out");
            in = addInPort<bool>("in");
        }

        /**
         * Internal transition: occurs after output command is sent.
         * Resets send_command flag and increments command index if input is active.
         */
        void internalTransition(ServoCommandState &state) const override
        {
            state.send_command = false; // Reset the send command flag

            // If input is true and more commands are available, advance to next command
            if (state.input && state.index + 1 < state.commands.size())
            {
                state.index++;
                state.speed = state.commands[state.index];
                state.sigma = 1.0; // Schedule next internal event in 1 second
            }
        }

        /**
         * External transition: updates input flag based on received messages.
         * Sets speed command or disables it accordingly.
         */
        void externalTransition(ServoCommandState &state, double e) const override
        {
            if (!in->empty())
            {
                for (const auto value : in->getBag())
                {
                    state.input = value;
                }

                if (state.input)
                {
                    state.send_command = true;
                    state.speed = state.commands[state.index];
                    state.sigma = 1.0; // Trigger internal transition in 1 second
                }
                else
                {
                    state.speed = -0.5; // Negative value signals stopping or idle
                    state.sigma = 1.0;
                }
            }
        }

        /**
         * Output function: sends the current speed command on the output port.
         */
        void output(const ServoCommandState &state) const override
        {
            out->addMessage(state.speed);
        }

        /**
         * Time advance function: returns time until next internal transition.
         */
        [[nodiscard]] double timeAdvance(const ServoCommandState &state) const override
        {
            return state.sigma;
        }
    };
}

#endif // SERVO_COMMAND_GENERATOR_HPP
