/*
This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
and released into public domain.
*/

#ifndef CPPCRYPTO_HASH_H
#define CPPCRYPTO_HASH_H

#include <stdint.h>
#include <string>
#include <initializer_list>

namespace cppcrypto
{

class crypto_hash
{
public:
	crypto_hash() {}
	virtual ~crypto_hash();

	virtual void init() = 0;
	virtual void update(const unsigned char* data, size_t len) = 0;
	virtual void final(unsigned char* hash) = 0;

	virtual size_t hashsize() const = 0;
	virtual size_t blocksize() const = 0;
	virtual crypto_hash* clone() const = 0;
	virtual void clear() = 0;

	void hash_string(const unsigned char* data, size_t len, unsigned char* hash);
	void hash_string(const char* data, size_t len, unsigned char* hash);
	void hash_string(const std::string& data, unsigned char* hash);

protected:
	void validate_hash_size(size_t hs, size_t max) const;
	void validate_hash_size(size_t hs, std::initializer_list<size_t> set) const;

private:
	crypto_hash(const crypto_hash&) = delete;
	void operator=(const crypto_hash&) = delete;
};

}

#endif
