#ifndef MBASE_CRYPTO_INIT_H
#define MBASE_CRYPTO_INIT_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <Windows.h>
#include <bcrypt.h>
#include <iostream>
#include <ntstatus.h>

#pragma comment(lib, "bcrypt.lib")

// NOTES
// WIN32 CRYPTO PROGRAMMING
/* 

Opening the Algorithm Provider (CRYPTO INIT SETS UP PROVIDERS)
Getting or Setting Algorithm Properties (CRYPTO INIT SETS UP PROPERTIES)
Creating or Importing a Key
Performing Cryptographic Operations
Closing the Algorithm Provider

*/

MBASE_STD_BEGIN

enum class crypto_error : U64 {
	MBASE_CRYERR_SUCCESS = 0,
	MBASE_CRYERR_NOT_SUPPORTED = 1,
	MBASE_CRYERR_INVALID_CALL = 2,
	MBASE_CRYERR_INTERNAL_ERROR = 3,
	MBASE_CRYERR_INVALID_SIZE = 4
};

struct crypto_provider_data {
	BCRYPT_ALG_HANDLE providerHandle;
	U64 objectSize;
	mbase::string providerName;
};

struct crypto_init {
	// ASSUME THEY WILL 100% SUCCEED FOR NOW
	crypto_init() noexcept {
		NTSTATUS ntStat;

		ntStat = BCryptOpenAlgorithmProvider(&hash_MD5.providerHandle, BCRYPT_MD5_ALGORITHM, nullptr, 0);
		if(ntStat != STATUS_SUCCESS)
		{
			// NOTIFY DIAGNOSTICS
		}

		ntStat = BCryptOpenAlgorithmProvider(&hash_SHA256.providerHandle, BCRYPT_SHA256_ALGORITHM, nullptr, 0);
		if (ntStat != STATUS_SUCCESS)
		{
			// NOTIFY DIAGNOSTICS
		}

		ntStat = BCryptOpenAlgorithmProvider(&symm_AES.providerHandle, BCRYPT_AES_ALGORITHM, nullptr, 0);
		if (ntStat != STATUS_SUCCESS)
		{
			// NOTIFY DIAGNOSTICS
		}
		ntStat = BCryptOpenAlgorithmProvider(&asym_RSA.providerHandle, BCRYPT_RSA_ALGORITHM, nullptr, 0);
		if (ntStat != STATUS_SUCCESS)
		{
			// NOTIFY DIAGNOSTICS
		}

		ntStat = BCryptOpenAlgorithmProvider(&rng_rng.providerHandle, BCRYPT_RNG_ALGORITHM, nullptr, 0);
		if (ntStat != STATUS_SUCCESS)
		{
			// NOTIFY DIAGNOSTICS
		}

		ULONG pcbResult = 0;

		BCryptGetProperty(hash_MD5.providerHandle, BCRYPT_OBJECT_LENGTH, (PBYTE)&hash_MD5.objectSize, sizeof(U64), &pcbResult, 0);
		BCryptGetProperty(hash_SHA256.providerHandle, BCRYPT_OBJECT_LENGTH, (PBYTE)&hash_SHA256.objectSize, sizeof(U64), &pcbResult, 0);
		BCryptGetProperty(symm_AES.providerHandle, BCRYPT_OBJECT_LENGTH, (PBYTE)&symm_AES.objectSize, sizeof(U64), &pcbResult, 0);

		// ASYM WILL BE IMPLEMENTED LATER
	}
	~crypto_init() noexcept {
		// no need to check the validity of the provider handles

		BCryptCloseAlgorithmProvider(rng_rng.providerHandle, 0);
		BCryptCloseAlgorithmProvider(asym_RSA.providerHandle, 0);
		BCryptCloseAlgorithmProvider(symm_AES.providerHandle, 0);
		BCryptCloseAlgorithmProvider(hash_SHA256.providerHandle, 0);
		BCryptCloseAlgorithmProvider(hash_MD5.providerHandle, 0);
	}

	crypto_provider_data hash_MD5 = { 0 };
	crypto_provider_data hash_SHA256 = { 0 };
	crypto_provider_data symm_AES = { 0 };
	crypto_provider_data asym_RSA = { 0 };
	crypto_provider_data rng_rng = { 0 };
};

using crypto_manager = crypto_init;

static crypto_manager gCryptoManager;

MBASE_STD_END

#endif // MBASE_CRYPTO_INIT_H