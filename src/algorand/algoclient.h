#ifndef ALGO_CLIENT_H
#define ALGO_CLIENT_H

#include "ArduinoJson.h"


class AlgoClient
{
  public:
    AlgoClient(String v2Url, String v2IdxUrl, String apiKey);
    double getAccountBalance(String pubKey);
    DynamicJsonDocument getTransactions(String pubKey, int limit);
  private:
    String baseV2Url;
    String baseV2IndexerUrl;
    String apiKey;
    DynamicJsonDocument get(String serverPath);
};

#endif