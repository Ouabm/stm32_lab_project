#ifndef SAMPLE_TOP_HPP
#define SAMPLE_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "atomic.hpp"
#include "Digitalinput.hpp"
#include "Digitalout_good.hpp"
#include "Digitalout_bad.hpp"
#include "Digitalout_avrege.hpp"
#include "motionout.hpp"
#include "CO2polling.hpp"
#include "CO2reception.hpp"
#include "temperature.hpp"
#include "generator.hpp"
#include "controller.hpp"
#include "pwmoutput.hpp"
#include "stm32h7xx_hal_rcc.h"
#include "stm32h7xx_hal_dma.h"
#include "stm32h7xx_hal_uart.h"

extern "C"
{
#include "adc.h"
}

using namespace cadmium;

// Top-level coupled model containing all components and their connections
struct top_coupled : public Coupled
{
    top_coupled(const std::string &id) : Coupled(id)
    {
        // Enable GPIO clocks for ports A, B, G, E
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();

        // Add atomic_model component (likely the main logic or LED toggle model)
        auto atomique = addComponent<atomic_model>("atomique");

        // GPIO configurations for output LEDs and input pins
        static GPIO_InitTypeDef led_config_good = {
            .Pin = GPIO_PIN_0,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Alternate = 0};
        static GPIO_InitTypeDef led_config_avrege = {
            .Pin = GPIO_PIN_1,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Alternate = 0};
        static GPIO_InitTypeDef led_config_bad = {
            .Pin = GPIO_PIN_14,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Alternate = 0};

        static GPIO_InitTypeDef led_config_input = {
            .Pin = GPIO_PIN_0,
            .Mode = GPIO_MODE_INPUT,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Alternate = 0};

        static GPIO_InitTypeDef led_config_motion = {
            .Pin = GPIO_PIN_1,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Alternate = 0};

        // Define GPIO ports for components
        GPIO_TypeDef *led_port1 = GPIOB;
        GPIO_TypeDef *led_port2 = GPIOE;
        GPIO_TypeDef *led_port3 = GPIOG;
        GPIO_TypeDef *inputport = GPIOA;

        // Instantiate output components with their GPIO port and pin config
        auto digitaloutputgood = addComponent<DigitalOutputgood>(
            "digitaloutputgood",
            led_port1,
            &led_config_good);
        auto digitaloutputavrege = addComponent<DigitalOutputavrege>(
            "digitaloutputavrege",
            led_port2,
            &led_config_avrege);
        auto digitaloutputbad = addComponent<DigitalOutputbad>(
            "digitaloutputbad",
            led_port1,
            &led_config_bad);
        auto motionoutput = addComponent<DigitalOutput>(
            "motionoutput",
            led_port3,
            &led_config_motion);

        // Instantiate analog input component for ADC reading
        auto analogueinput = addComponent<AnalogInput>(
            "analogueinout",
            inputport,
            &hadc1);

        // Instantiate digital input component for motion detection
        auto motion = addComponent<DigitalInput>(
            "motion",
            led_port2,
            &led_config_input);

        // Instantiate other components handling CO2 data reception, temperature, servo control etc.
        auto reception = addComponent<Reception>("reception");
        auto temp = addComponent<TemperatureSensorInput>("Temp");
        auto generator = addComponent<ServoCommandGenerator>("ServocommandState");
        auto controller = addComponent<ServoController>("ServoCOntroller");
        auto pwm = addComponent<PWMOutput>("servoPWM", &htim4, TIM_CHANNEL_1, __HAL_TIM_GET_AUTORELOAD(&htim4));

        // Define connections between components (couplings)
        addCoupling(analogueinput->out, reception->in);
        addCoupling(reception->out_good, digitaloutputgood->in);
        addCoupling(reception->out_avrege, digitaloutputavrege->in);
        addCoupling(reception->out_bad, digitaloutputbad->in);
        addCoupling(temp->out, generator->in);
        addCoupling(generator->out, controller->in);
        addCoupling(controller->out, pwm->in);
        addCoupling(motion->out, motionoutput->in);
    }
};

#endif // SAMPLE_TOP_HPP
