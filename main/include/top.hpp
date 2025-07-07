#ifndef SAMPLE_TOP_HPP
#define SAMPLE_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "atomic.hpp"
#include "Digitalinput.hpp"
#include "Digitalout_good.hpp"
#include "Digitalout_bad.hpp"
#include "Digitalout_avrege.hpp"
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

struct top_coupled : public Coupled
{
    top_coupled(const std::string &id) : Coupled(id)
    {
        // __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();

        auto atomique = addComponent<atomic_model>("atomique");

        // Configuration manuelle du GPIO
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
            .Pin = GPIO_PIN_14,
            .Mode = GPIO_MODE_INPUT,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Alternate = 0};

        GPIO_TypeDef *led_port1 = GPIOB;
        GPIO_TypeDef *led_port2 = GPIOE;
        GPIO_TypeDef *inputport = GPIOA;

        // Ajout du composant DigitalOutput avec paramètres
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
        auto analogueinput = addComponent<AnalogInput>(
            "analogueinout",
            inputport,
            &hadc1);
        
        auto motion = addComponent<DigitalInput>(
            "motion",
            led_port2,
            &led_config_input);

        auto reception = addComponent<Reception>("reception");
        auto temp = addComponent<TemperatureSensorInput>("Temp");
        auto generator = addComponent<ServoCommandGenerator>("ServocommandState");
        auto controller = addComponent<ServoController>("ServoCOntroller");
        auto pwm = addComponent<PWMOutput>("servoPWM", &htim4, TIM_CHANNEL_1, __HAL_TIM_GET_AUTORELOAD(&htim4));
        addCoupling(analogueinput->out, reception->in);
        addCoupling(reception->out_good, digitaloutputgood->in);
        addCoupling(reception->out_avrege, digitaloutputavrege->in);
        addCoupling(reception->out_bad, digitaloutputbad->in);
        addCoupling(temp->out, generator->in);
        addCoupling(generator->out, controller->in);
        addCoupling(controller->out, pwm->in);
        // addCoupling(motion->out, digitaloutputbad->in);
    }
};

#endif // SAMPLE_TOP_HPP
