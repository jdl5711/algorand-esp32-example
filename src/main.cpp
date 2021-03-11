#include <Arduino.h>
#include <Constants.h>
#include "wifi_setup.h"
#include "algorand/account.h"
#include "algorand/algoclient.h"
#include "CommandProcessor.h"


extern "C" {
    #include "crypto/base64.h"
}

String PUBLIC_KEY;
AlgoClient *CLIENT = new AlgoClient(Constants::BASE_URL, Constants::IDX_BASE_URL, Constants::API_KEY);
CommandProcessor *CMD_PROC = new CommandProcessor(CLIENT);
/*
* The setup funtion will be called once after boot.  Here we initialze
* WIFI and retrieve our public key. 
*/
void setup() {
  //Setting up serial output. This number should match monitor_speed in platformio.ini
  Serial.begin(115200);  
  
  setupWifi(Constants::MY_SSID.c_str(), Constants::MY_PASSWORD.c_str());
  
  //Create an account object and retrieve the public key
  Account *account = new Account();
  PUBLIC_KEY = account->getPublicKey();
  
}

/*
*  Arduino will run this method in a loop.  All recurring tasks go here. 
*  In this method we will monitor our Algorand account 
*  for balance changes and device commands. 
*/
void loop() {
  Serial.println("Using Address:");
  Serial.println(PUBLIC_KEY);
  //1) Lets check our balance for more ALGOS.  
  double balance = CLIENT->getAccountBalance(PUBLIC_KEY);
  Serial.print("Balance = ");
  Serial.print(balance);
  Serial.println(" ALGOS");

  /* 
    2) Check for any new commands
        In our scenario, a command is an instruction from a 
        user on the Algorand blockchain sent to our IoT 
        device via the note field.  
  */
  CMD_PROC->processCommands(PUBLIC_KEY);
  delay(15000);
}

/*
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

TaskHandle_t ledTaskHandle = NULL;
void processLedCmd(String cmd){
  //first kill any running task and make sure the light is off.
  if(ledTaskHandle != NULL) {
    Serial.println("stopping task");
    vTaskDelete(ledTaskHandle);
    ledTaskHandle = NULL;
    digitalWrite(LED_PIN, LOW);
  }

  if(cmd.equalsIgnoreCase("blink-fast")){ //(void*)500
    xTaskCreate(toggleLed, "ToggleFastLED", 1000, (void*)&FAST_LED, 1, &ledTaskHandle);
  } else if(cmd.equalsIgnoreCase("blink-slow")) {
    xTaskCreate(toggleLed, "ToggleSlowLED", 1000, (void*)&SLOW_LED, 1, &ledTaskHandle);
    //function, name, stack size, parameter, task priority, handle
  } else if(!cmd.equalsIgnoreCase("stop")) {
    Serial.println("Try again. Received unrecognized LED command: " + cmd);
  }
}

void processCmd(String cmd){
  if(cmd.equalsIgnoreCase("reboot") || cmd.equalsIgnoreCase("restart")) {
    ESP.restart();
  } else {
    Serial.println("Received unknown cmd: " + cmd);
  }
}

String txID;
void processCommands() {
  //We assume the last transaction contains the last known state of the device.
  // So we only care about the last transation (i.e. limit=1)  
  DynamicJsonDocument doc = client->getTransactions(pubKey, 1);
  try {
        if(doc != NULL && doc["transactions"][0] != NULL 
            && !txID.equalsIgnoreCase(doc["transactions"][0]["id"].as<String>()) ) {
            // Set this transaction ID so we don't process it again
            txID = doc["transactions"][0]["id"].as<String>();
            size_t outlen ;
            String note64 = doc["transactions"][0]["note"];
            if(note64 != NULL) {
              unsigned char* decodedNote = base64_decode(((const unsigned char *)note64.c_str()), note64.length(), &outlen);
              //Important! We assume the note is received in JSON format.
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
*/
