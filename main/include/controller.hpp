#ifndef SERVO_CONTROLLER_HPP
#define SERVO_CONTROLLER_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <limits>
#include <iostream>
#include <algorithm>

namespace cadmium
{

    struct ServoControllerState
    {
        double duty;
        double sigma;

        ServoControllerState() : duty(0.0), sigma(std::numeric_limits<double>::infinity()) {}
    };

    std::ostream &operator<<(std::ostream &out, const ServoControllerState &state)
    {
        out << "Duty cycle: " << state.duty;
        return out;
    }

    class ServoController : public Atomic<ServoControllerState>
    {
    public:
        Port<double> in;
        Port<double> out;

        ServoController(const std::string &id) : Atomic<ServoControllerState>(id, ServoControllerState())
        {
            in = addInPort<double>("in");
            out = addOutPort<double>("out");
        }

        void internalTransition(ServoControllerState &state) const override
        {
            state.sigma = std::numeric_limits<double>::infinity();
        }

        void externalTransition(ServoControllerState &state, double e) const override
        {
            if (!in->empty())
            {
                for (const auto &angle : in->getBag())
                {
                    state.duty = angleToDutyCycle(angle);
                }
                state.sigma = 0.0;
            }
        }

        void output(const ServoControllerState &state) const override
        {
            out->addMessage(state.duty);
        }

        [[nodiscard]] double timeAdvance(const ServoControllerState &state) const override
        {
            return state.sigma;
        }

    private:
        static double angleToDutyCycle(double angle)
        {
            if (angle == -0.5)
            {
                return 0.0;
            }
            else
            {
                angle = std::clamp(angle, 0.0, 180.0);
                double min_duty = 0.05;
                double max_duty = 0.10;
                return min_duty + (angle / 180.0) * (max_duty - min_duty);
            }
        }
    };
}

#endif // SERVO_CONTROLLER_HPP
