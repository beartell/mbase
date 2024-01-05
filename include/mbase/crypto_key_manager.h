#ifndef MBASE_CRYPTO_KEY_MNG_H
#define MBASE_CRYPTO_KEY_MNG_H

#include <mbase/common.h>
#include <mbase/crypto_init.h>
#include <mbase/rng.h>
#include <mbase/behaviors.h>

MBASE_STD_BEGIN

class crypto_key_manager : public non_copyable {
public:
	using key_handle = BCRYPT_KEY_HANDLE;

	struct sym_key_out {
		// KEY OBJECT LENGTH STORED INSIDE crypto_provider_data : objectSize
		BCRYPT_KEY_HANDLE mkHandle = nullptr;
		PUCHAR keyObject = nullptr;
	};

	explicit crypto_key_manager(crypto_provider_data& in_provider) noexcept {
		ULONG writtenByteCount = 0;
		mProvider = &in_provider;
		
		if (!BCryptGetProperty(in_provider.providerHandle, BCRYPT_BLOCK_LENGTH, (PUCHAR)&secretLength, sizeof(U64), &writtenByteCount, 0))
		{
			secretBuffer = new IBYTE[secretLength];
			gen_random_n(secretBuffer, secretLength);
		}
	}

	~crypto_key_manager() noexcept {
		if (secretBuffer)
		{
			delete [] secretBuffer;
		}
	}

	USED_RETURN MBASE_INLINE crypto_error generate_symmetric_key(sym_key_out* out_key) const noexcept {
		if (!secretLength)
		{
			return crypto_error::MBASE_CRYERR_NOT_SUPPORTED;
		}

		BCRYPT_KEY_HANDLE tempHandle = nullptr;
		PUCHAR tempKeyObject = new UCHAR[mProvider->objectSize];

		if (BCryptGenerateSymmetricKey(mProvider->providerHandle, &tempHandle, tempKeyObject, mProvider->objectSize, (PUCHAR)secretBuffer, secretLength, 0))
		{
			return crypto_error::MBASE_CRYERR_INVALID_CALL;
		}

		out_key->mkHandle = tempHandle;
		out_key->keyObject = tempKeyObject;

		return crypto_error::MBASE_CRYERR_SUCCESS;
	}

	MBASE_INLINE GENERIC regenerate_secret() noexcept {
		gen_random_n(secretBuffer, secretLength);
	}

	USED_RETURN MBASE_INLINE U64 get_block_length() const noexcept {
		return secretLength;
	}

	USED_RETURN MBASE_INLINE U64 get_secret_length() const noexcept {
		return secretLength;
	}

private:
	IBYTEBUFFER secretBuffer = nullptr;
	crypto_provider_data* mProvider = nullptr; // mem, managed by some other class possibly crypto_symmetric()
	U64 secretLength = 0;
};

MBASE_STD_END

#endif // !MBASE_CRYPTO_KEY_MNG_H
