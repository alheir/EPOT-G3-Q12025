// #include <Arduino.h>
// #include "driver/mcpwm.h"
// #include <math.h>

// #define PWM_FREQ       20000       // Carrier frequency (Hz)
// #define DEAD_TIME_NS   1000        // 1 µs dead time
// #define SINE_STEPS     360

// // Output pins (high and low for each phase)
// #define PHASE_A_H 25
// #define PHASE_A_L 26
// #define PHASE_B_H 27
// #define PHASE_B_L 14
// #define PHASE_C_H 32
// #define PHASE_C_L 33

// // SPWM control parameters
// float mf = 50.0;     // Output frequency (Hz)
// float ma = 0.8;      // Modulation index (0..1)
// float angle = 0.0;
// float angleStep;

// uint16_t sineTable[SINE_STEPS];

// hw_timer_t *timer = NULL;
// portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// void setupSineTable() {
//   for (int i = 0; i < SINE_STEPS; i++) {
//     float rad = 2 * PI * i / SINE_STEPS;
//     sineTable[i] = (uint16_t)((sin(rad) * 0.5 + 0.5) * 100);  // 0–100% duty
//   }
// }

// void setupMCPWMTimer(mcpwm_timer_t timer, mcpwm_io_signals_t sigA, gpio_num_t pinA, mcpwm_io_signals_t sigB, gpio_num_t pinB) {
//   mcpwm_gpio_init(MCPWM_UNIT_0, sigA, pinA);  // High
//   mcpwm_gpio_init(MCPWM_UNIT_0, sigB, pinB);  // Low

//   mcpwm_config_t config = {
//     .frequency = PWM_FREQ,
//     .cmpr_a = 0,
//     .cmpr_b = 0,
//     .duty_mode = MCPWM_DUTY_MODE_0,
//     .counter_mode = MCPWM_UP_DOWN_COUNTER
//   };
//   mcpwm_init(MCPWM_UNIT_0, timer, &config);

//   mcpwm_deadtime_enable(MCPWM_UNIT_0, timer,
//     MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, DEAD_TIME_NS, DEAD_TIME_NS);
// }

// void IRAM_ATTR onTimer() {
//   portENTER_CRITICAL_ISR(&timerMux);

//   angle += angleStep;
//   if (angle >= SINE_STEPS) angle -= SINE_STEPS;

//   int ia = (int)angle % SINE_STEPS;
//   int ib = (ia + SINE_STEPS / 3) % SINE_STEPS;
//   int ic = (ia + 2 * SINE_STEPS / 3) % SINE_STEPS;

//   float da = sineTable[ia] * ma;
//   float db = sineTable[ib] * ma;
//   float dc = sineTable[ic] * ma;

//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, da);
//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, da);

//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, db);
//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B, db);

//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_A, dc);
//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_OPR_B, dc);

//   portEXIT_CRITICAL_ISR(&timerMux);
// }

// void setup() {
//   setupSineTable();

//   setupMCPWMTimer(MCPWM_TIMER_0, MCPWM0A, (gpio_num_t)PHASE_A_H, MCPWM0B, (gpio_num_t)PHASE_A_L);
//   setupMCPWMTimer(MCPWM_TIMER_1, MCPWM1A, (gpio_num_t)PHASE_B_H, MCPWM1B, (gpio_num_t)PHASE_B_L);
//   setupMCPWMTimer(MCPWM_TIMER_2, MCPWM2A, (gpio_num_t)PHASE_C_H, MCPWM2B, (gpio_num_t)PHASE_C_L);

//   angleStep = mf * SINE_STEPS / 10000.0; // For 10 kHz update

//   timer = timerBegin(0, 80, true); // 1 µs tick
//   timerAttachInterrupt(timer, &onTimer, true);
//   timerAlarmWrite(timer, 100, true); // 100 µs = 10 kHz
//   timerAlarmEnable(timer);
// }

// void loop() {
//   delay(1000);
// }
