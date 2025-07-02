#ifndef RT_TEMPERATURESENSORINPUT_HPP
#define RT_TEMPERATURESENSORINPUT_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include "stm32h7xx_hal_gpio.h" 
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal.h"

extern "C" {
#include "DHT_11/DHT.h"
#include "tim.h"
}

#ifndef NO_LOGGING
#include <iostream>
#include <fstream>
#include <string>
#endif

using namespace std;

namespace cadmium {

// État du capteur
struct TemperatureSensorInputState {
    bool output;
    double sigma;
    float Temperature;
     float lastTemperature;

    TemperatureSensorInputState() : output(false), sigma(0.0),Temperature(100){}
};

// Surcharge de l'opérateur de flux pour debug / log
inline std::ostream& operator<<(std::ostream& out, const TemperatureSensorInputState& state) {
    out << "Temperature: " << state.Temperature;
    return out;
}

class TemperatureSensorInput : public Atomic<TemperatureSensorInputState> {
public:
    Port<bool> out;
  

    TemperatureSensorInput(const std::string& id)
    : Atomic<TemperatureSensorInputState>(id, TemperatureSensorInputState()) {
      
        out = addOutPort<bool>("out");
       

        
    }

    void internalTransition(TemperatureSensorInputState& state) const override {
         uint8_t RHI, RHD, TCI, TCD, SUM;
         float tCelsius = 0.0f;

          if (DHT11_Start()) {
        
              RHI = DHT11_Read();
              RHD = DHT11_Read();
              TCI = DHT11_Read();
              TCD = DHT11_Read();
              SUM = DHT11_Read();
              if ((uint8_t)(RHI + RHD + TCI + TCD) == SUM) {
                  tCelsius = TCI + (TCD / 10.0f);
                  
    state.Temperature = tCelsius;
              } else {
                  state.Temperature = 100; // erreur checksum
              }
          } else {
              state.Temperature = 100; // erreur démarrage capteur
          }
        if(state.Temperature<=25){
            state.output= false;
        }
        else{
            state.output=true;
        }
        state.sigma=2.0;
        
    }

    void externalTransition(TemperatureSensorInputState& state, double e) const override {
        // Pas d'input externe géré ici
        (void)state;
        (void)e;
    }

    void output(const TemperatureSensorInputState& state) const override {
       
        out->addMessage(state.output);
    }

    [[nodiscard]] double timeAdvance(const TemperatureSensorInputState& state) const override {
        return state.sigma;
    }
};

} // namespace cadmium

#endif // RT_TEMPERATURESENSORINPUT_HPP
