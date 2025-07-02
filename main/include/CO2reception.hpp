#ifndef RT_CO2reception
#define RT_CO2reception_HPP

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_dac.h"
extern "C" {
     #include "adc.h"
    }

using namespace std;

namespace cadmium {

    struct ReceptionState {
        float input;
        bool output_good;
        bool output_bad;
        bool output_avrege;
        double sigma;

        ReceptionState(): input(0.0),output_good(false), output_bad(false),output_avrege(false) ,sigma(0) {}
    };

    std::ostream& operator<<(std::ostream &out, const ReceptionState& state) {
    
        return out;
    }

    class Reception : public Atomic<ReceptionState> {
    public:
    
        Port<bool> out_good;
        Port<bool> out_avrege;
        Port<bool> out_bad;
        Port<float> in;
       

        // Période d’échantillonnage (en secondes)
        double pollingRate;

        // Constructeur
        Reception(const std::string& id) 
            : Atomic<ReceptionState>(id, ReceptionState()) {

            out_good = addOutPort<bool>("out_good");
            out_avrege = addOutPort<bool>("out_avrege");
            out_bad = addOutPort<bool>("out_bad");
            in =addInPort<float>("in");
                   
        }

        // Transition interne : lire une nouvelle valeur ADC
        void internalTransition(ReceptionState& state) const override {

           if(state.input>=1000.0){
            state.output_bad=true;
            state.output_good=false;
            state.output_avrege=false;
           }
           else if(state.input<1000 && state.input>=500){
            state.output_avrege=true;
            state.output_good=false;
            state.output_bad=false;
           }
           else{
            state.output_good=true;
            state.output_avrege=false;
            state.output_bad=false;
           }
            state.sigma = 0.1;
        }

        // Transition externe
        void externalTransition(ReceptionState& state, double e) const override {
            if (!in->empty()) {
                for (const auto value : in->getBag()) {
                    state.input = value;
                }
            } 
        }

        // Fonction de sortie
        void output(const ReceptionState& state) const override {
           
                out_good->addMessage(state.output_good);
                out_avrege->addMessage(state.output_avrege);
                out_bad->addMessage(state.output_bad);
            
        }

        // Avance dans le temps
        [[nodiscard]] double timeAdvance(const ReceptionState& state) const override {
            return state.sigma;
        }
    };
}

#endif // RT_ANALOGINPUT_HPP
