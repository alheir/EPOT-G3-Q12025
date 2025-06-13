// ------- SPWM Generator --------------------------- //
//  Grupo: Cílfone, Di Sanzo, Figueroa, Gioia, Heir
// 22.28 - Electrónica de Potencia - ITBA
// ---------------------------------------------------//

#include <Arduino.h>
#include "pwm_gen.h"

bool var_freq = true;  // Variable frequency mode
void setup()
{
  Serial.begin(115200);  // Initialize serial communication
  init_mcpwm();
  start_gen();
  set_freq(20.0);  // Set initial frequency
}

void loop()
{
  static unsigned long lastPrint = 0;
  static int smoothed_adc = 0;
  static const float alpha = 0.8;  // Smoothing factor: 0 < alpha <= 1

  unsigned long now = millis();

  // Print and update once every 500 ms
  if (now - lastPrint > 100) {
    lastPrint = now;

    if (var_freq) {
      int adc_value = analogRead(34);
      smoothed_adc = alpha * adc_value + (1 - alpha) * smoothed_adc;

      int new_freq = map(smoothed_adc, 0, 4095, 20, 100);
      int current_freq = (int)get_freq();

      if (abs(new_freq - current_freq) > 5 && abs(new_freq - current_freq) < 20 && new_freq >= 20 && new_freq <= 100) {
        Serial.print("New frequency: ");
        Serial.println(new_freq);
        Serial.print("Last frequency: ");
        Serial.println(current_freq);
        set_freq(new_freq);
      }
    }

    Serial.print("Current Frequency: ");
    Serial.println(get_freq());
  }
}
