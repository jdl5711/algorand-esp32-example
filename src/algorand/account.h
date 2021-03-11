
#ifndef Account_h
#define Account_h


class Account
{
  public:
    Account();
    String getPublicKey(void);    
  private:
    void init(void);
    String generateKeys(void);    
    void createAlgorandPrivateKey(const uint8_t *private_key, const uint8_t *public_key);
    String createAlgorandPublicKey(const uint8_t *public_key);
    String publicKey;
    String privateKey;
  
};

#endif