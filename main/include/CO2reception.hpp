#ifndef RT_CO2reception
#define RT_CO2reception_HPP

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_dac.h"
extern "C"
{
#include "adc.h"
}

using namespace std;

namespace cadmium
{

    // State structure for the Reception model
    struct ReceptionState
    {
        float input;           // Received CO₂ value (in ppm)
        bool output_good;      // Flag if CO₂ level is considered good
        bool output_bad;       // Flag if CO₂ level is considered bad
        bool output_avrege;    // Flag if CO₂ level is considered average
        double sigma;          // Time until next internal transition

        ReceptionState() : input(0.0), output_good(false), output_bad(false), output_avrege(false), sigma(0) {}
    };

    // Optional: print state to log (not used here)
    std::ostream &operator<<(std::ostream &out, const ReceptionState &state)
    {
        return out;
    }

    // Atomic model class to categorize CO₂ levels
    class Reception : public Atomic<ReceptionState>
    {
    public:
        Port<bool> out_good;     // Output port: CO₂ level is good
        Port<bool> out_avrege;   // Output port: CO₂ level is average
        Port<bool> out_bad;      // Output port: CO₂ level is bad
        Port<float> in;          // Input port: receives CO₂ value

        double pollingRate;      // Unused, reserved for future extension

        // Constructor: define ports and initialize state
        Reception(const std::string &id)
            : Atomic<ReceptionState>(id, ReceptionState())
        {
            out_good = addOutPort<bool>("out_good");
            out_avrege = addOutPort<bool>("out_avrege");
            out_bad = addOutPort<bool>("out_bad");
            in = addInPort<float>("in");
        }

        // Internal transition: evaluate CO₂ level and update output flags
        void internalTransition(ReceptionState &state) const override
        {
            if (state.input >= 1000.0)
            {
                // High CO₂ level: considered bad
                state.output_bad = true;
                state.output_good = false;
                state.output_avrege = false;
            }
            else if (state.input < 1000 && state.input >= 500)
            {
                // Medium CO₂ level: considered average
                state.output_avrege = true;
                state.output_good = false;
                state.output_bad = false;
            }
            else
            {
                // Low CO₂ level: considered good
                state.output_good = true;
                state.output_avrege = false;
                state.output_bad = false;
            }

            state.sigma = 0.1; // Schedule next evaluation shortly
        }

        // External transition: receive new CO₂ input
        void externalTransition(ReceptionState &state, double e) const override
        {
            if (!in->empty())
            {
                for (const auto value : in->getBag())
                {
                    state.input = value; // Take latest input value
                }
            }
        }

        // Output function: send category flags to respective output ports
        void output(const ReceptionState &state) const override
        {
            out_good->addMessage(state.output_good);
            out_avrege->addMessage(state.output_avrege);
            out_bad->addMessage(state.output_bad);
        }

        // Time advance function: return delay before next internal transition
        [[nodiscard]] double timeAdvance(const ReceptionState &state) const override
        {
            return state.sigma;
        }
    };
}

#endif // RT_CO2reception_HPP
