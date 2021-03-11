/*
This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
and released into public domain.
*/

#include "algorand/sha/crypto_hash.h"
#include <stdexcept>
#include <algorithm>

namespace cppcrypto
{
	crypto_hash::~crypto_hash()
	{
	}

	void crypto_hash::hash_string(const char* data, size_t len, unsigned char* hash)
	{
		hash_string((const unsigned char*)data, len, hash);
	}

	void crypto_hash::hash_string(const unsigned char* data, size_t len, unsigned char* hash)
	{
		init();
		update((const unsigned char*)data, len);
		final(hash);
	}

	void crypto_hash::hash_string(const std::string& data, unsigned char* hash)
	{
		hash_string((const unsigned char*)data.c_str(), data.length(), hash);
	}

	void crypto_hash::validate_hash_size(size_t hs, std::initializer_list<size_t> set) const
	{
		if (!hs)
			throw std::runtime_error("hash size can't be zero");

		if (std::find(set.begin(), set.end(), hs))
			return;

		throw std::runtime_error("invalid hash size");
	}

	void crypto_hash::validate_hash_size(size_t hs, size_t max) const
	{
		if (!hs)
			throw std::runtime_error("hash size can't be zero");

		if (hs % 8)
			throw std::runtime_error("cppcrypto does not support non-byte hash sizes");

		if (hs > max)
			throw std::runtime_error("invalid hash size");
	}

}

