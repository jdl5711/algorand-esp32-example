// This code is written by kerukuro for cppcrypto library (http://cppcrypto.sourceforge.net/)
// and released into public domain.


//#ifndef CPPCRYPTO_SHA512_H
#define CPPCRYPTO_SHA512_H

#include "algorand/sha/crypto_hash.h"
#include "algorand/sha/alignedarray.h"
#include <array>
#include <functional>

namespace cppcrypto
{

	class sha512 : public crypto_hash
	{
	public:
		sha512(size_t hashsize = 512);
		~sha512();

		void init() override;
		void update(const unsigned char* data, size_t len) override;
		void final(unsigned char* hash) override;

		size_t hashsize() const override { return hs; }
		size_t blocksize() const override { return 1024; }
		sha512* clone() const override { return new sha512; }
		void clear() override;

	protected:
		void transform(void* m, uint64_t num_blks);

		std::function<void(void*, uint64_t)> transfunc;
		aligned_pod_array<uint64_t, 8, 32> H;
		std::array<unsigned char, 128> m;
		size_t pos;
		uint64_t total;
		size_t hs;
	};

	class sha384 : public sha512
	{
	public:
		sha384() : sha512(384) {}

		sha384* clone() const override { return new sha384; }
	};

}

//#endif
