#include <Arduino.h>
#include "pwm_gen.h"

bool var_freq = true;  // Variable frequency mode
void setup()
{
  Serial.begin(115200);  // Initialize serial communication
  init_mcpwm();
  start_gen();
  set_freq(70.0);  // Set initial frequency
}

void loop()
{
  static unsigned long lastPrint = 0;
  static int last_freq_int = -1;
  unsigned long now = millis();

  // Print once per second
  if (now - lastPrint > 2000) {
    lastPrint = now;
    if (var_freq) {
      int adc_value = analogRead(34);
      Serial.print("ADC Value: ");
      Serial.println(adc_value);
      int new_freq = map(adc_value, 0, 4095, 50, 200);
      Serial.print("Mapped Frequency: ");
      Serial.println(new_freq);
      if (new_freq-(int)get_freq() > -5 || new_freq-(int)get_freq() < 5) {
        set_freq(new_freq);
      }
    }

    Serial.print("Current Frequency: ");
    Serial.println(get_freq());

  }
}
