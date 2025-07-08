#ifndef SERVO_CONTROLLER_HPP
#define SERVO_CONTROLLER_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <limits>
#include <iostream>
#include <algorithm>

namespace cadmium
{

    // Structure to represent the internal state of the servo controller
    struct ServoControllerState
    {
        double duty;   // PWM duty cycle corresponding to the servo angle
        double sigma;  // Time until the next internal transition

        ServoControllerState() : duty(0.0), sigma(std::numeric_limits<double>::infinity()) {}
    };

    // Optional debug print for logging the state
    std::ostream &operator<<(std::ostream &out, const ServoControllerState &state)
    {
        out << "Duty cycle: " << state.duty;
        return out;
    }

    // Atomic DEVS model to convert input angle into PWM duty cycle
    class ServoController : public Atomic<ServoControllerState>
    {
    public:
        Port<double> in;   // Input port: receives servo angle in degrees
        Port<double> out;  // Output port: sends PWM duty cycle

        // Constructor: initialize ports and state
        ServoController(const std::string &id) : Atomic<ServoControllerState>(id, ServoControllerState())
        {
            in = addInPort<double>("in");
            out = addOutPort<double>("out");
        }

        // Internal transition: nothing to do after sending output, so we deactivate the model
        void internalTransition(ServoControllerState &state) const override
        {
            state.sigma = std::numeric_limits<double>::infinity();
        }

        // External transition: receives new angle and updates the duty cycle
        void externalTransition(ServoControllerState &state, double e) const override
        {
            if (!in->empty())
            {
                for (const auto &angle : in->getBag())
                {
                    state.duty = angleToDutyCycle(angle); // Convert angle to PWM duty cycle
                }
                state.sigma = 0.0; // Schedule immediate output
            }
        }

        // Output function: sends the computed duty cycle to the output port
        void output(const ServoControllerState &state) const override
        {
            out->addMessage(state.duty);
        }

        // Time advance function: return time until next internal event
        [[nodiscard]] double timeAdvance(const ServoControllerState &state) const override
        {
            return state.sigma;
        }

    private:
        // Converts angle in degrees [0,180] to PWM duty cycle [5%, 10%]
        static double angleToDutyCycle(double angle)
        {
            if (angle == -0.5)
            {
                // Special case: -0.5 disables the servo output (duty = 0)
                return 0.0;
            }
            else
            {
                // Clamp angle to valid range [0, 180]
                angle = std::clamp(angle, 0.0, 180.0);

                double min_duty = 0.05; // Corresponds to 0°
                double max_duty = 0.10; // Corresponds to 180°
                
                // Linear interpolation between min and max duty cycle
                return min_duty + (angle / 180.0) * (max_duty - min_duty);
            }
        }
    };
}

#endif // SERVO_CONTROLLER_HPP
