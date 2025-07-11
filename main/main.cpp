#include "include/top.hpp"
#include "cadmium/simulation/root_coordinator.hpp"
#include "cadmium/simulation/rt_root_coordinator.hpp"
#include "cadmium/simulation/logger/stdout.hpp"
#include "cadmium/simulation/rt_clock/stm32_rt_clock.hpp"

extern "C"
{
#include "stm32h7xx_hal.h"
#include "tim.h"
}

int main()
{
  MX_TIM2_Init();             // Initialize timer 2 (generated by CubeMX)
  HAL_TIM_Base_Start(&htim2); // Start timer 2 in base mode

  MX_TIM4_Init();                           // Initialize timer 4 (generated by CubeMX)
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); // Start PWM on timer 4 channel 1

  MX_TIM6_Init();             // Initialize timer 6 (generated by CubeMX)
  HAL_TIM_Base_Start(&htim6); // Start timer 6 in base mode

  MX_ADC1_Init(); // Initialize ADC1

  // Create a shared instance of the main coupled model "top_coupled"
  auto model = std::make_shared<top_coupled>("top_coupled");

  STM32Clock<double> clock; // Create STM32 real-time clock object

  // Create Cadmium real-time root coordinator with the model and STM32 clock
  auto rootCoordinator = cadmium::RealTimeRootCoordinator<STM32Clock<double>>(model, clock);

  rootCoordinator.setLogger<cadmium::STDOUTLogger>(";"); // Set logger to write to standard output

  rootCoordinator.start(); // Start the simulation

  rootCoordinator.simulate(10000.0); // Run simulation for 10,000 time units (ms, s, depending on definition)

  rootCoordinator.stop(); // Stop the simulation

  return 0;
}