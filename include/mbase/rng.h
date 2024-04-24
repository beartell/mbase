#ifndef MBASE_CRYPTO_RNG_H
#define MBASE_CRYPTO_RNG_H

#include <mbase/common.h>
#include <mbase/crypto_init.h>

MBASE_STD_BEGIN

MBASE_ND("randomdly generated number being unused") MBASE_INLINE I32 gen_random_32() noexcept {
	I32 freshNumber = 0;
	BCryptGenRandom(gCryptoManager.rng_rng.providerHandle, (PUCHAR)&freshNumber, sizeof(I32), 0);
	return freshNumber;
}

MBASE_ND("randomdly generated number being unused") MBASE_INLINE I64 gen_random_64() noexcept {
	I64 freshNumber = 0;
	BCryptGenRandom(gCryptoManager.rng_rng.providerHandle, (PUCHAR)&freshNumber, sizeof(I64), 0);
	return freshNumber;
}

MBASE_ND("randomly generated sequence being unused") MBASE_INLINE GENERIC gen_random_n(IBYTEBUFFER in_buffer, SIZE_T in_length) noexcept {
	BCryptGenRandom(gCryptoManager.rng_rng.providerHandle, (PUCHAR)in_buffer, in_length, 0);
}

MBASE_STD_END

#endif //