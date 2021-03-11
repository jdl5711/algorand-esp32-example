
#include <HardwareSerial.h>
#include "algorand/algoclient.h"
#include "ArduinoJson.h"
#include <HTTPClient.h>

extern "C" {
    #include "crypto/base64.h"
}

/**
 * The AlgoClient is primarily responsible for managing APIs to interact 
 * with the blockchain 
 */ 
AlgoClient::AlgoClient(String v2Url, String v2IdxUrl, String aKey){
    baseV2Url = v2Url;
    baseV2IndexerUrl = v2IdxUrl;
    apiKey = aKey;
}

/**
 * Retrieve account information and extract the balance
 * String publicKey
 * return double balance
 */ 
double AlgoClient::getAccountBalance(String pubKey) {
    String serverPath = baseV2Url + "accounts/"+pubKey;
    DynamicJsonDocument doc = get(serverPath);
    double balance = doc["amount"];
    if(balance > 0) {
        balance = balance/1000000;
    }
    return balance;
}

/**
 * Retrieve transactions from the Indexer
 * String publicKey
 * int limit -- the number of results to return
 */ 
DynamicJsonDocument AlgoClient::getTransactions(String pubKey, int limit){
    String serverPath = baseV2IndexerUrl + "accounts/"+ pubKey 
                        + "/transactions?limit=" + limit;
    return get(serverPath);
}

/**
 * Common method for API GET requests 
 */
DynamicJsonDocument AlgoClient::get(String serverPath){
    DynamicJsonDocument *doc = NULL;
    if(WiFi.status()== WL_CONNECTED) {
        try {
            HTTPClient http;

            http.begin(serverPath.c_str());
            http.addHeader("x-api-key", apiKey);
        
            // Send HTTP GET request
            int httpResponseCode = http.GET();
        
            if (httpResponseCode>0) {
                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);
                String payload = http.getString();
                
                doc = new DynamicJsonDocument(payload.length());
                deserializeJson(*doc, payload);
                //Serial.println(payload);
            }
            else {
                Serial.print("Error code: ");
                Serial.println(httpResponseCode);
            }
            // Free resources
            http.end();            
        } 
        catch(const std::exception& e){
            Serial.print(e.what());
        }
        return *doc;
    }
    else {
      Serial.println("WiFi Disconnected");
      return *doc;
    }
}

 