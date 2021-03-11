#include <sodium.h>
#include <HardwareSerial.h>
#include <string>
#include "ArduinoNvs.h"
#include "algorand/sha/sha512.h"
#include "algorand/encoding/base32.h"
#include "base64.h"
#include "account.h"

/** Strings to persist and retrieve the public/private keys from non-volitale storage */
const static String PUB_KEY = "pubK";
const static String PRI_KEY = "priK";

/**
 *  The role of the Account class is to generate the public/private key pair and 
 *  make it available to the main program to execute transactions, retrieve balance, etc.
 */
Account::Account(){
    init();
}

/**
 * Return the public key for this device. If one does not exist
 * generate a new keypair and persist it to the device for future transactions. 
 */
String Account::getPublicKey(void){
    return this->publicKey;
}

/**
 * Initialize the Account. Retrieve the public/private keys. If they don't
 * exist, generate them.
 */ 
void Account::init(void) {
    // 1) look in non-volitale storage for the account. 
    NVS.begin();
    this->publicKey = NVS.getString(PUB_KEY);
    this->privateKey = NVS.getString(PRI_KEY);
    //If we don't have both the public and private key, regenerate.
    if(this->publicKey == NULL || this->privateKey == NULL) {
        this->publicKey = generateKeys();
    } else {
        Serial.println("Public:");
        Serial.println(publicKey);
        Serial.println("Private:");
        Serial.println(privateKey);
    } 
}

/**
 * Generate the public/private key pairs with Ed25519. Then apply Algorand rules
 * to create keys acceptable to the blockchain.
 * 
 * Return: public key
 */
String Account::generateKeys(void){
    Serial.println(F("Beginning key gen"));
    
    //Algorand defines public and private keys as 32 bytes
    uint8_t private_key[32];
    uint8_t public_key[32];

    // First, generate the Ed25519 public/private key pair
    unsigned char seed[crypto_sign_SEEDBYTES];
    randombytes(seed, crypto_sign_SEEDBYTES);
    crypto_sign_ed25519_seed_keypair(public_key, private_key, seed);

    // Second, evolve the keys into Algorand keys.
    createAlgorandPrivateKey(private_key, public_key);
    String pubKey = createAlgorandPublicKey(public_key);

    return pubKey;
}

/**
 * Apply Algorand rules for the private key:  
 *      (1) Concatenate private + public key
 *      (2) base64 encode
 *      (3) Persist on device
 */
void Account::createAlgorandPrivateKey(const uint8_t *private_key, const uint8_t *public_key){
    uint8_t combined_private[64];
    memcpy(combined_private, private_key, 32);
    memcpy(combined_private+32, public_key, 32);

    base64 base64;
    String private_key_encoded = base64.encode(combined_private, sizeof(combined_private));

    Serial.println(F("PrivateKey:"));
    Serial.println(private_key_encoded);
    this->privateKey = private_key_encoded;
    NVS.setString(PRI_KEY, private_key_encoded);
}

/**
 *  Apply the public key tranformation defined by Algorand.
 *  1) Create a SHA-512/256 hash from the public key
 *  2) Append the last four bytes of the hash to the end of the public key
 *  3) Base32 Encode the new string
 *  4) Trim any extra bytes/padding from the conversion, resulting in a 58 character String
*/
String Account::createAlgorandPublicKey(const uint8_t *public_key){
    // Step 1: generate a SHA-512/256 hash from the public key
    uint8_t public_hash[32];
    cppcrypto::sha512 sha512(256);
    sha512.init();
    sha512.update(public_key, 32);
    sha512.final(public_hash);
    
    // Step 2: Algorand expects the last 4 bytes of the hash to be appended to the public_key making
    // it 36 bytes
    uint8_t *tmp_hash = public_hash + sizeof(public_hash) - 4;
    uint8_t public_key_final[36];  
    memcpy(public_key_final, public_key, 32);
    memcpy(public_key_final+32, tmp_hash, 4);

    // Step 3: The public_key is then base32 encoded  
    Base32 b32encoder;
    uint8_t *public_key_encoded = NULL;
    b32encoder.toBase32(public_key_final, 36, public_key_encoded, true);
    
    //Step 4: Need only the 58 chars (terminated by EOL) which is the Algorand Base32 encoded key
    String pubk = (String(reinterpret_cast< char const* >(public_key_encoded))).substring(0,58);
    
    Serial.println(F("PublicKey:"));
    Serial.println(pubk);
    this->publicKey = pubk;
    NVS.setString(PUB_KEY, pubk);
    return pubk;
}

/*
*  UNTESTED
* Retrieve the public key from the private key
String Account::getPublicFromPrivate() {
    String privKey = NVS.getString(PRI_KEY);
    unsigned char *pubKey ;
    if(privKey != NULL) {
        unsigned char *pubKey ;
        unsigned char *sKey = ((unsigned char *)privKey.c_str()); 
        int kp = crypto_sign_ed25519_sk_to_pk(pubKey, sKey);
    }
    String strKey = (String(reinterpret_cast< char const* >(pubKey)));
    Serial.println("got pubk: ");
    Serial.println(strKey);
    return strKey;
}
*/