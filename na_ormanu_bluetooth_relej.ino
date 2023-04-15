#include "BluetoothSerial.h"
#include <Arduino.h>

#define DISABLE_CODE_FOR_RECEIVER // Disables restarting receiver after each send. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not used.
//#define SEND_PWM_BY_TIMER         // Disable carrier PWM generation in software and use (restricted) hardware PWM.
//#define USE_NO_SEND_PWM           // Use no carrier PWM, just simulate an active low receiver signal. Overrides SEND_PWM_BY_TIMER definition

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
#include <IRremote.hpp>





BluetoothSerial ESP_BT; //Object for Bluetooth


//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3        /* Time ESP32 will go to sleep (in seconds) */
#define repeats 2
RTC_DATA_ATTR int bootCount = 0;
BluetoothSerial SerialBT;
const int relayPin =  2;
// Handle received and sent messages
String message = "";
char incomingChar;

void setup() {
  pinMode(relayPin, OUTPUT);

  SerialBT.begin("ESP32test"); //Bluetooth device name


  IrSender.begin(DISABLE_LED_FEEDBACK); // Start with IR_SEND_PIN as send pin and disable feedback LED at default feedback LED pin

}
void loop() {

  if (SerialBT.available()) {
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n') {
      message += String(incomingChar);
    }
    else {
      message = "";
    }
  }

  if (!(message == "")) {

    if (message == "ukljuci") {
      digitalWrite(relayPin, HIGH);
      delay(10000);
      IrSender.sendSamsung(0x1010, 0x1E, repeats); //Power On button
      delay(3000);
      IrSender.sendSamsung(0x1010, 0x9, repeats); //Aux button
      delay(1000);

    }
    else if (message == "iskljuci") {
      digitalWrite(relayPin, LOW);
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      esp_deep_sleep_start();
    }
    else if (digitalRead(relayPin) == HIGH) {

      if (message == "LG_VOLUME_UP") {
        IrSender.sendSamsung(0x1010, 0x17, repeats);
        delay(200);
      }
      else if (message == "LG_VOLUME_DOWN") {
        IrSender.sendSamsung(0x1010, 0x16, repeats);
        delay(200);
      }

    }

  }

}