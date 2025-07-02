#ifndef RT_ANALOGINPUT_HPP
#define RT_ANALOGINPUT_HPP

#include "cadmium/modeling/devs/atomic.hpp"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_adc.h"
#include "stm32h7xx_hal_dac.h"
extern "C" {
     #include "adc.h"
    }

using namespace std;

namespace cadmium {

    struct AnalogInputState {
        float output;
        double sigma;
         float tab[21] = {0.0f};
        int index = 0;
        AnalogInputState(): output(0.0), sigma(1.0) {}
    };

    std::ostream& operator<<(std::ostream &out, const AnalogInputState& state) {
        out << "Valeur analogique: " << state.output;
        return out;
    }

    class AnalogInput : public Atomic<AnalogInputState> {
    public:
        Port<float> out;

        // Pointeur vers le handle ADC
        ADC_HandleTypeDef* analogPin;
        GPIO_TypeDef* port;
        
       

        // Période d’échantillonnage (en secondes)
        double pollingRate;

        // Constructeur
        AnalogInput(const std::string& id,  GPIO_TypeDef* selectedPort,ADC_HandleTypeDef* pin) 
            : Atomic<AnalogInputState>(id, AnalogInputState()),port(selectedPort) ,analogPin(pin), pollingRate(1.0) {

            out = addOutPort<float>("out");
                   
        }

        // Transition interne : lire une nouvelle valeur ADC
        void internalTransition(AnalogInputState& state) const override {

            HAL_ADC_Start(analogPin);
            HAL_ADC_PollForConversion(analogPin, 20); // Attente conversion
	        uint16_t raw = HAL_ADC_GetValue(analogPin);
	        float voltage = (raw / 1024.0) * 5;
            float Vout = voltage / 8.5f;
	    	
    state.tab[state.index] = Vout;
    state.index = (state.index + 1) % 21;

    float sum = 0.0f;
    for(int i = 0; i < 21; i++) {
        sum += state.tab[i];
    }
	    	float Vref = sum / 21;
	        float ppm = 0.0f;
	        float correction;
	        float ppm_c;
	        float slope = 0.030f / (2.602f - 3.0f);  
	         ppm = powf(10.0f, ((Vout - Vref) / slope + 2.602f));
	        // correction = 2.731;
	        // ppm_c=ppm*correction;
             state.output=ppm;
            state.sigma = 0.8;
        }

        // Transition externe (non utilisée ici)
        void externalTransition(AnalogInputState& state, double e) const override {
            // Pas d’entrée, aucune action à effectuer ici
        }

        // Fonction de sortie
        void output(const AnalogInputState& state) const override {
           
                out->addMessage(state.output);
            
        }

        // Avance dans le temps
        [[nodiscard]] double timeAdvance(const AnalogInputState& state) const override {
            return state.sigma;
        }
    };
}

#endif // RT_ANALOGINPUT_HPP
