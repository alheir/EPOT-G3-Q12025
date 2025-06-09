// #include <Arduino.h>
// #include "driver/mcpwm.h"

// #define A_HIGH 15
// #define A_LOW   2
// #define B_HIGH  4
// #define B_LOW  16
// #define C_HIGH 17
// #define C_LOW   5

// #define POTENTIOMETER_PIN 34
// #define FREQ_MIN 5.0
// #define FREQ_MAX 50.0

// #define SINE_STEPS 1000
// float freq = 100.0;
// float vdc = 310.0;
// float vf_ratio = 10.0;
// float ma = 0.8;
// uint16_t sineA[SINE_STEPS], sineB[SINE_STEPS], sineC[SINE_STEPS];
// volatile int sineIndex = 0;
// hw_timer_t *timer = NULL;

// void generateSineTables() {
//   for (int i = 0; i < SINE_STEPS; i++) {
//     float angle = 2 * PI * i / SINE_STEPS;
//     float base = ma * 50.0;
//     sineA[i] = 50 + base * sin(angle);
//     sineB[i] = 50 + base * sin(angle - 2 * PI / 3);
//     sineC[i] = 50 + base * sin(angle + 2 * PI / 3);
//   }
// }

// void IRAM_ATTR onTimer() {
//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, sineA[sineIndex]);
//   mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A, sineB[sineIndex]);
//   mcpwm_set_duty(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, sineC[sineIndex]);
//   sineIndex = (sineIndex + 1) % SINE_STEPS;
// }

// void setupPWM() {
//   mcpwm_config_t pwm_config = {
//     .frequency = 20000,
//     .cmpr_a = 50.0,
//     .cmpr_b = 50.0,
//     .duty_mode = MCPWM_DUTY_MODE_0,
//     .counter_mode = MCPWM_UP_DOWN_COUNTER
//   };

//   // Fase A
//   mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, A_HIGH);
//   mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, A_LOW);
//   mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
//   mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 100, 100);

//   // Fase B
//   mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, B_HIGH);
//   mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, B_LOW);
//   mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);
//   mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 100, 100);

//   // Fase C
//   mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, C_HIGH);
//   mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0B, C_LOW);
//   mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);
//   mcpwm_deadtime_enable(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 100, 100);
// }

// float readPotentiometerFreq() {
//   int adc = analogRead(POTENTIOMETER_PIN);
//   return FREQ_MIN + ((float)adc / 4095.0) * (FREQ_MAX - FREQ_MIN);
// }

// void updateSPWM(float newFreq) {
//   freq = newFreq;
//   ma = vf_ratio * freq / vdc;
//   if (ma > 1.0) ma = 1.0;
//   generateSineTables();

//   int updateRate = freq * SINE_STEPS;
//   int period_us = 1e6 / updateRate;
//   timerAlarmWrite(timer, period_us, true);
// }

// void setup() {
//   Serial.begin(115200);
//   analogReadResolution(12);
//   pinMode(POTENTIOMETER_PIN, INPUT);
//   setupPWM();
//   generateSineTables();

//   timer = timerBegin(0, 80, true);
//   timerAttachInterrupt(timer, &onTimer, true);
//   int updateRate = freq * SINE_STEPS;
//   timerAlarmWrite(timer, 1e6 / updateRate, true);
//   timerAlarmEnable(timer);
// }

// void loop() {
//   float newFreq = readPotentiometerFreq();
//   if (abs(newFreq - freq) > 0.2) {
//     updateSPWM(newFreq);
//     Serial.print("Freq = ");
//     Serial.print(freq);
//     Serial.print(" Hz | m_a = ");
//     Serial.println(ma);
//   }

//   delay(100);
// }