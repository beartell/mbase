#ifndef MBASE_CRYPTO_SYM_H
#define MBASE_CRYPTO_SYM_H

#include <mbase/common.h>
#include <mbase/crypto_init.h>
#include <mbase/crypto_key_manager.h>
#include <mbase/safe_buffer.h>

MBASE_STD_BEGIN

class crypto_symmetric_aes {
public:
	crypto_symmetric_aes() noexcept : ckm(gCryptoManager.symm_AES) {}

	USED_RETURN MBASE_INLINE crypto_error encrypt(safe_buffer* in_src, safe_buffer* out_target) const noexcept {
		U64 blockLength = ckm.get_block_length();
		if(!blockLength)
		{
			return crypto_error::MBASE_CRYERR_NOT_SUPPORTED;
		}

		crypto_key_manager::sym_key_out symko;
		ckm.generate_symmetric_key(&symko);
		
		ULONG pcbRes = 0;
		// PASS NULL TO CALCULATE SIZE
		NTSTATUS encryptResult = BCryptEncrypt(symko.mkHandle,
			(PUCHAR)in_src->bfSource,
			in_src->bfLength,
			NULL,
			NULL,
			0,
			symko.keyObject,
			gCryptoManager.symm_AES.objectSize,
			&pcbRes,
			BCRYPT_BLOCK_PADDING
		);
		
		std::cout << pcbRes << std::endl;

		if(encryptResult)
		{
			printf("%x\n", encryptResult);
			return crypto_error::MBASE_CRYERR_INVALID_CALL;
		}

		out_target->bfSource = (IBYTEBUFFER)symko.keyObject;
		out_target->bfLength = blockLength;

		return crypto_error::MBASE_CRYERR_SUCCESS;
	}

private:
	crypto_key_manager ckm;
};

MBASE_STD_END

#endif //