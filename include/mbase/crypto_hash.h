#ifndef MBASE_CRYPTO_HASH_H
#define MBASE_CRYPTO_HASH_H

#include <mbase/common.h>
#include <mbase/crypto_init.h>
#include <mbase/safe_buffer.h>
#include <mbase/string.h>

MBASE_STD_BEGIN

class crypto_hash_base : public non_copyable{
public:
	crypto_hash_base(crypto_provider_data& in_provider) noexcept {
		ULONG pcbRes = 0;
		U64 calcLength = 0;
		NTSTATUS cryptRes = BCryptGetProperty(in_provider.providerHandle,
			BCRYPT_HASH_LENGTH,
			(PUCHAR)&calcLength,
			sizeof(U64),
			&pcbRes,
			0
		);
		
		PUCHAR hashObject = new UCHAR[in_provider.objectSize];

		cryptRes = BCryptCreateHash(in_provider.providerHandle,
			&ourHashHandle,
			hashObject,
			in_provider.objectSize,
			nullptr,
			0,
			BCRYPT_HASH_REUSABLE_FLAG
		);
		
		hashObj = hashObject;
		hashSize = calcLength;
	}

	~crypto_hash_base() noexcept {
		if(ourHashHandle)
		{
			delete[] hashObj;
			BCryptDestroyHash(ourHashHandle);
		}
	}

	USED_RETURN MBASE_INLINE crypto_error hash_data(IBYTEBUFFER in_src, SIZE_T in_length, safe_buffer* out_buffer) const noexcept {
		return _hash_data(in_src, in_length, out_buffer);
	}

	USED_RETURN MBASE_INLINE crypto_error hash_data(const mbase::string& in_src, safe_buffer* out_buffer) const noexcept {
		return _hash_data(in_src.c_str(), in_src.size(), out_buffer);
	}

private:
	PUCHAR hashObj = nullptr;
	BCRYPT_HASH_HANDLE ourHashHandle = nullptr;
	U64 hashSize;

	USED_RETURN MBASE_INLINE crypto_error _hash_data(IBYTEBUFFER in_src, SIZE_T in_length, safe_buffer* out_buffer) const noexcept {
		U64 hashValue = 0;
		NTSTATUS cryptRes = BCryptHashData(ourHashHandle, (PUCHAR)in_src, in_length, 0);
		PUCHAR outHash = new UCHAR[hashSize];
		cryptRes = BCryptFinishHash(ourHashHandle, outHash, hashSize, 0);
		if (cryptRes)
		{
			delete[] outHash;
			return crypto_error::MBASE_CRYERR_INTERNAL_ERROR;
		}
		
		out_buffer->bfSource = (IBYTEBUFFER)outHash;
		out_buffer->bfLength = hashSize;
		
		return crypto_error::MBASE_CRYERR_SUCCESS;
	}
};

class crypto_hash_md5 : public crypto_hash_base {
public:
	crypto_hash_md5() noexcept : crypto_hash_base(gCryptoManager.hash_MD5) {}
};

class crypto_hash_sha256 : public crypto_hash_base {
public:
	crypto_hash_sha256() noexcept : crypto_hash_base(gCryptoManager.hash_SHA256) {}
};

MBASE_STD_END

#endif //