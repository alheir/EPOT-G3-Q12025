// ------- SPWM Generator --------------------------- //
//  Grupo: Cílfone, Di Sanzo, Figueroa, Gioia, Heir
// 22.28 - Electrónica de Potencia - ITBA
// ---------------------------------------------------//

#include <Arduino.h>
#include "pwm_gen.h"

bool var_freq = true;  // Variable frequency mode
bool adc_mode = true;  // true = ADC mode, false = Serial mode

void print_help() {
  Serial.println("\n========== SPWM Generator Control ==========");
  Serial.println("Commands:");
  Serial.println("  help          - Show this help");
  Serial.println("  mode adc      - Switch to ADC (potentiometer) mode");
  Serial.println("  mode serial   - Switch to Serial input mode");
  Serial.println("  freq <value>  - Set frequency (15-99 Hz) in serial mode");
  Serial.println("  status        - Show current status");
  Serial.println("  start         - Start PWM generation");
  Serial.println("  stop          - Stop PWM generation");
  Serial.println("===========================================\n");
}

// void print_status() {
//   Serial.println("\n========== Current Status ==========");
//   Serial.print("PWM Generation: ");
//   Serial.println(var_freq ? "ENABLED" : "DISABLED");
//   Serial.print("Control Mode: ");
//   Serial.println(adc_mode ? "ADC (Potentiometer)" : "Serial Input");
//   Serial.print("Current Frequency: ");
//   Serial.print(get_freq(), 2);
//   Serial.println(" Hz");
//   Serial.print("Modulation Index: ");
//   Serial.print(get_ma(), 3);
//   Serial.print("Modulation Ratio: ");
//   Serial.print(get_mf(), 1);
//   Serial.println("\n===================================\n");
// }

void process_serial_command() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "help") {
      print_help();
    }
    else if (command == "status") {
      // print_status();
    }
    else if (command == "start") {
      var_freq = true;
      start_gen();
      Serial.println("PWM generation STARTED");
    }
    else if (command == "stop") {
      var_freq = false;
      stop_gen();
      Serial.println("PWM generation STOPPED");
    }
    else if (command.startsWith("mode ")) {
      String mode = command.substring(5);
      if (mode == "adc") {
        adc_mode = true;
        Serial.println("Switched to ADC (Potentiometer) mode");
        Serial.println("Use potentiometer on pin 34 to control frequency");
      }
      else if (mode == "serial") {
        adc_mode = false;
        Serial.println("Switched to Serial input mode");
        Serial.println("Use 'freq <value>' command to set frequency");
      }
      else {
        Serial.println("ERROR: Invalid mode. Use 'mode adc' or 'mode serial'");
      }
    }
    else if (command.startsWith("freq ")) {
      if (!adc_mode) {
        String freq_str = command.substring(5);
        float new_freq = freq_str.toFloat();
        
        if (new_freq >= 15.0 && new_freq <= 99.0) {
          set_freq(new_freq);
          Serial.print("Frequency set to: ");
          Serial.print(new_freq, 2);
          Serial.println(" Hz");
        }
        else {
          Serial.println("ERROR: Frequency must be between 15.0 and 99.0 Hz");
        }
      }
      else {
        Serial.println("ERROR: Switch to serial mode first with 'mode serial'");
      }
    }
    else if (command != "") {
      Serial.println("ERROR: Unknown command. Type 'help' for available commands");
    }
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  

  Serial.begin(115200);
  delay(250);
  
  init_mcpwm();
  delay(250);
  
  set_freq(21);  // Set initial frequency to 21 Hz
  delay(250);
  
  start_gen();
  delay(250);
  
  // Print welcome message
  Serial.println("\n======================================");
  Serial.println("    SPWM Generator - EPOT G3 2025");
  Serial.println("======================================");
  print_help();
  // print_status();
}

void loop()
{
  static unsigned long lastPrint = 0;
  static unsigned long lastADC = 0;
  static int smoothed_adc = 0;
  static const float alpha = 0.3;  // Smoothing factor for ADC

  unsigned long now = millis();
  
  // Process serial commands
  process_serial_command();

  if(now - lastADC > 200)
  {
    lastADC = now;
    // Blink LED to indicate activity
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    float current_freq = get_freq();
    Serial.print("Current Frequency: ");
    Serial.print(current_freq, 2);
    Serial.println(" Hz");
  }

  // ADC mode processing (every 50ms)
  if (adc_mode && var_freq && (now - lastADC > 500)) {
    lastADC = now;
    
    int adc_value = analogRead(34);
    smoothed_adc = (int)(alpha * (float)adc_value + (1 - alpha) * (float)smoothed_adc);
    
    float new_freq = (float)map(smoothed_adc, 0, 4095, 15, 99);
    if(new_freq < 15.0) new_freq = 15.0;
    if(new_freq > 99.0) new_freq = 99.0;

    float current_freq = get_freq();
    
    // Only update if change is significant (reduces jitter)
    if (abs(new_freq - current_freq) > 1.0) {
      set_freq(new_freq);
      Serial.print("Frequency set to: ");
      Serial.print(new_freq, 2);
      Serial.println(" Hz");
    }
  }

  // // Status printing (every 2 seconds, only in ADC mode)
  // if (adc_mode && var_freq && (now - lastPrint > 2000)) {
  //   lastPrint = now;
    
  //   Serial.print("ADC Mode - Freq: ");
  //   Serial.print(get_freq(), 1);
  //   Serial.print(" Hz, ADC: ");
  //   Serial.print(smoothed_adc);
  //   Serial.print(", ma: ");
  //   Serial.print(get_ma(), 3);
  //   Serial.print(", mf: ");
  //   Serial.println(get_mf(), 1);
  // }
}
