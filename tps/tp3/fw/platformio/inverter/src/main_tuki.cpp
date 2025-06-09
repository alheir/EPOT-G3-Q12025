#include <Arduino.h>
#include "pwm_gen.h"

void setup()
{
  Serial.begin(115200);  // Initialize serial communication
  init_mcpwm();
  start_gen();
  set_mf(111);
}

void loop()
{
  static unsigned long lastPrint = 0;
  static int last_freq_int = -1;
  unsigned long now = millis();

  int adc_value = analogRead(34);
  
  // Map ADC to a freq range, say 1 Hz to 100 Hz
  float new_freq = map(adc_value, 0, 4095, 1, 100);  // integer mapping
//   if (new_freq - last_freq_int > 5) {
//     set_freq((float)new_freq);  // set new frequency
//     last_freq_int = new_freq;
//   }

  // Print once per second
  if (now - lastPrint > 1000) {
    lastPrint = now;

    float current_freq = get_freq();
    int current_mf = get_mf();
    float output_freq = current_freq * current_mf;

    Serial.print("Pot ADC: ");
    Serial.print(adc_value);
    Serial.print(" | Fundamental freq: ");
    Serial.print(current_freq);
    Serial.print(" Hz | mf: ");
    Serial.print(current_mf);
    Serial.print(" | Output freq: ");
    Serial.print(output_freq);
    Serial.println(" Hz");
  }
}
