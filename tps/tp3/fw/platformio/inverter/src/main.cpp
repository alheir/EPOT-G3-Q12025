// ------- SPWM Generator --------------------------- //
//  Grupo: Cílfone, Di Sanzo, Figueroa, Gioia, Heir
// 22.28 - Electrónica de Potencia - ITBA
// ---------------------------------------------------//

#include <Arduino.h>
#include "pwm_gen.h"

bool var_freq = true;  // Variable frequency mode
void setup()
{
  //Serial.begin(115200);  // Initialize //Serial communication
  init_mcpwm();
  start_gen();
  set_freq(70);  // Set initial frequency
}

void loop()
{
  static unsigned long lastPrint = 0;
  static int smoothed_adc = 0;
  static const float alpha = 0.3;  // Smoothing factor: 0 < alpha <= 1

  unsigned long now = millis();

  // Print and update once every 500 ms
  if (now - lastPrint > 50) {
    lastPrint = now;
    float current_freq = get_freq();
    if (var_freq) {
      int adc_value = analogRead(34);
      smoothed_adc = (int)(alpha * (float)adc_value + (1 - alpha) * (float)smoothed_adc);
      //Serial.print("ADC Value: ");
      //Serial.println(adc_value);
      float new_freq = (float) map(smoothed_adc, 0, 4095, 20, 100);
      //Serial.print("Reading frequency: ");
      //Serial.println(new_freq);
      //if (abs(new_freq - current_freq) > 1 && new_freq >= 0 && new_freq <= 200) {
        //Serial.print("Last frequency: ");
        //Serial.println(current_freq);
        set_freq(new_freq);
      //}
    }
    //current_freq = get_freq();
    //Serial.print("Current Frequency: ");
    //Serial.println(current_freq);
  }
}
