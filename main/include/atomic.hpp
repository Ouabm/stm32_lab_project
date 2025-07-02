#ifndef ATOMIC_MODEL_HPP
#define ATOMIC_MODEL_HPP
#include "cadmium/modeling/devs/atomic.hpp"


using namespace cadmium;

struct atomic_modelState {

    double sigma;
    bool led_on;
    bool fastToggle;
     bool buttonPressed;  // nouveau
    atomic_modelState() : sigma(0.0), led_on(false),fastToggle(true),buttonPressed(false) {}
};

std::ostream& operator<<(std::ostream& out, const atomic_modelState& state) {
   out << "Status:, " << state.led_on << ", sigma: " << state.sigma;
    return out;
}

class atomic_model : public Atomic<atomic_modelState> {
public:
    Port<bool>out ;
    Port<bool> in;
    double slowToggleTime ;
    double fastToggleTime;
    // Constructeur : initialiser la LED sur la broche LED1
    atomic_model(const std::string& id) :
        Atomic<atomic_modelState>(id, atomic_modelState())
    {
        out = addOutPort<bool>("out");
        in= addInPort<bool>("in");
        slowToggleTime = 10;
        fastToggleTime = 1;
        state.sigma=fastToggleTime;

            // Set a value for sigma (so it is not 0), this determines how often the
       
    }

    void internalTransition(atomic_modelState& state) const override {
        state.led_on=!state.led_on;
         state.sigma = state.fastToggle ? fastToggleTime : slowToggleTime;
    }

    void externalTransition(atomic_modelState& state, double e) const override {
        if (!in->empty()) {
        for (const auto x : in->getBag()) {
            if (x == false && !state.buttonPressed) { 
                // bouton vient d’être appuyé
                state.fastToggle = !state.fastToggle;
                state.buttonPressed = true; // verrouillage tant que bouton enfoncé
                break;
            }
            if (x == true) {
                // bouton relâché -> on peut réactiver prise en compte
                state.buttonPressed = false;
            }
        }
        state.sigma = state.fastToggle ? fastToggleTime : slowToggleTime;
    }
    }

    void output(const atomic_modelState& state) const override {
        out->addMessage(state.led_on);// Pas besoin d’output explicite ici, car l’action se fait dans internalTransition
         
    }

    [[nodiscard]] double timeAdvance(const atomic_modelState& state) const override {
        return state.sigma;
    }
};
#endif
