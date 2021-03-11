
#include <Arduino.h>
#include "ArduinoJson.h"
#include "CommandProcessor.h"

//The GPIO PIN associated to the LED on the 
// ESP32 DevKit v1 is 2
const int LED_PIN = 2;
//The rate (in milliseconds) that the LED will flash
const int FAST_LED = 350;
const int SLOW_LED = 1200;

TaskHandle_t ledTaskHandle = NULL;
void toggleLed(void * parameter){
  // Start an infinite loop. 
  // This will run until the the task is killed by a new transaction/note
  for(;;){ 
    // Turn the LED on then pause
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(*((int*)parameter) / portTICK_PERIOD_MS);
    // Turn the LED off then pause
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(*((int*)parameter) / portTICK_PERIOD_MS);
  }
}

/**
 * Initialize the AlgoClient and set the GPIO mode 
 * to output for the PIN associated with LEDs and we make sure the LED
 * begins in an OFF state.  
 */ 
CommandProcessor::CommandProcessor(AlgoClient *algoClient){
    client = algoClient;
    pinMode (LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
}


/**
 *  The processCommands function will retrieve the last transaction for the provided publicKey.
 *  It then parses the note field looking for a JSON object with an instruction for the device.
 *  We assume the last transaction contains the last known state of the device so whatever command
 *  exists we will reset device state accordingly. 
 */
void CommandProcessor::processCommands(String pubKey) {
  //
  // So we only care about the last transation (i.e. limit=1)  
  DynamicJsonDocument doc = client->getTransactions(pubKey, 1);
  try {
        if(doc != NULL && doc["transactions"][0] != NULL 
            && !this->txID.equalsIgnoreCase(doc["transactions"][0]["id"].as<String>()) ) {
            // Set this transaction ID so we don't process it again
            this->txID = doc["transactions"][0]["id"].as<String>();
            size_t outlen ;
            String note64 = doc["transactions"][0]["note"];
            if(note64 != NULL) {
              unsigned char* decodedNote = base64_decode(((const unsigned char *)note64.c_str()), note64.length(), &outlen);
              //Important! No defensive programming. We assume the note is received in JSON format.
              DynamicJsonDocument noteDoc(64);
              deserializeJson(noteDoc, decodedNote);
              if(noteDoc.containsKey("led")) {
                processLedCmd(noteDoc["led"].as<String>());
              }
              else if(noteDoc.containsKey("cmd"))  {
                processCmd(noteDoc["cmd"].as<String>());
              }
            }
        }
    } catch(const std::exception& e){
        Serial.print(e.what());
    } catch(...){
        Serial.print("catch all");
    } 
}

/**
 * The proecessLedCmd is responsible for processing an LED command received 
 * from the transaction. 
 */ 
void CommandProcessor::processLedCmd(String cmd){
  //first kill any running task and make sure the light is off.
  if(ledTaskHandle != NULL) {    
    vTaskDelete(ledTaskHandle);
    ledTaskHandle = NULL;
    digitalWrite(LED_PIN, LOW);
  }

  if(cmd.equalsIgnoreCase("blink-fast")){ 
    Serial.println("Received command to blink-fast");
    xTaskCreate(toggleLed, "ToggleFastLED", 1000, (void*)&FAST_LED, 1, &ledTaskHandle);
  } else if(cmd.equalsIgnoreCase("blink-slow")) {
    Serial.println("Received command to blink-slow");
    xTaskCreate(toggleLed, "ToggleSlowLED", 1000, (void*)&SLOW_LED, 1, &ledTaskHandle);
    //function, name, stack size, parameter, task priority, handle
  } else if(!cmd.equalsIgnoreCase("stop")) {
    Serial.println("Try again. Received unrecognized LED command: " + cmd);
  } else {
    Serial.println("Stopped the blinking");
  }
}

/*
* Not tested. Don't use without additional defensive programming around the transaction ID
* otherwise this will result in constant reboots. 
*/
void CommandProcessor::processCmd(String cmd){
  if(cmd.equalsIgnoreCase("reboot") || cmd.equalsIgnoreCase("restart")) {
    //ESP.restart();
  } else {
    Serial.println("Received unknown cmd: " + cmd);
  }
}