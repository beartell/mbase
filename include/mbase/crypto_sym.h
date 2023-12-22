#ifndef MBASE_CRYPTO_SYM_H
#define MBASE_CRYPTO_SYM_H

#include <mbase/common.h>
#include <mbase/crypto_init.h>
#include <mbase/crypto_key_manager.h>
#include <mbase/safe_buffer.h>
#include <mbase/string.h>

MBASE_STD_BEGIN

class crypto_symmetric_base {
public:
	crypto_symmetric_base(IBYTEBUFFER in_key, SIZE_T in_len, crypto_provider_data& in_provider) noexcept : ckm(in_provider){
		if(import_key(in_key, in_len) != crypto_error::MBASE_CRYERR_SUCCESS)
		{
			ckm.generate_symmetric_key(&symko);
		}
	}

	crypto_symmetric_base(crypto_provider_data& in_provider) noexcept : ckm(in_provider) {
		ckm.generate_symmetric_key(&symko);
	}

	~crypto_symmetric_base() noexcept {
		if(symko.keyObject)
		{
			// MEANS KEY OBJECT INITIALIZED INSIDE THE KEY MANAGER
			BCryptDestroyKey(symko.mkHandle);
			delete[] symko.keyObject;
		}
	}

	USED_RETURN MBASE_INLINE crypto_error encrypt(const mbase::string& in_src, safe_buffer* out_target) const noexcept {
		return _encrypt(in_src.c_str(), in_src.size() + 1, out_target); // +1 is for null terminating character
	}

	USED_RETURN MBASE_INLINE crypto_error encrypt(IBYTEBUFFER in_src, SIZE_T in_length, safe_buffer* out_target) const noexcept {
		return _encrypt(in_src, in_length, out_target);
	}
	
	USED_RETURN MBASE_INLINE crypto_error decrypt(IBYTEBUFFER in_src, SIZE_T in_length, safe_buffer* out_target) const noexcept {
		return _decrypt(in_src, in_length, out_target);
	}

	USED_RETURN MBASE_INLINE crypto_error decrypt(IBYTEBUFFER in_src, SIZE_T in_length, mbase::string& out_string) const noexcept {
		safe_buffer myBuffer;
		crypto_error decErr = _decrypt(in_src, in_length, &myBuffer);

		if(decErr == crypto_error::MBASE_CRYERR_SUCCESS)
		{
			out_string = myBuffer.bfSource;
		}
		return decErr;
	}

	USED_RETURN MBASE_INLINE crypto_error export_key(safe_buffer* out_key) const noexcept {
		ULONG pcbRes = 0;
		NTSTATUS exportResult = 0;
		exportResult = BCryptExportKey(symko.mkHandle,
			nullptr,
			BCRYPT_KEY_DATA_BLOB,
			nullptr,
			0,
			&pcbRes,
			0);

		if(exportResult)
		{
			return crypto_error::MBASE_CRYERR_NOT_SUPPORTED;
		}

		IBYTEBUFFER myBuffer = new IBYTE[pcbRes];
		exportResult = BCryptExportKey(symko.mkHandle,
			nullptr,
			BCRYPT_KEY_DATA_BLOB,
			(PUCHAR)myBuffer,
			pcbRes,
			&pcbRes,
			0);
		if(exportResult)
		{
			return crypto_error::MBASE_CRYERR_INTERNAL_ERROR;
		}

		out_key->bfSource = myBuffer;
		out_key->bfLength = pcbRes;

		return crypto_error::MBASE_CRYERR_SUCCESS;
	}

	USED_RETURN MBASE_INLINE crypto_error import_key(IBYTEBUFFER in_src, SIZE_T in_length) noexcept {
		BCRYPT_KEY_HANDLE newKey;
		IBYTEBUFFER out_key = new IBYTE[gCryptoManager.symm_AES.objectSize];
		NTSTATUS importResult = BCryptImportKey(gCryptoManager.symm_AES.providerHandle,
			nullptr,
			BCRYPT_KEY_DATA_BLOB,
			&newKey,
			(PUCHAR)out_key,
			gCryptoManager.symm_AES.objectSize,
			(PUCHAR)in_src,
			in_length,
			0
		);

		if (symko.keyObject)
		{
			BCryptDestroyKey(symko.mkHandle);
			delete[] symko.keyObject;
		}

		symko.keyObject = (PUCHAR)out_key;
		symko.mkHandle = newKey;

		return crypto_error::MBASE_CRYERR_SUCCESS;
	}

private:
	USED_RETURN MBASE_INLINE crypto_error _encrypt(IBYTEBUFFER in_src, SIZE_T in_length, safe_buffer* out_target) const noexcept {
		U64 blockLength = ckm.get_block_length();
		if (!blockLength)
		{
			return crypto_error::MBASE_CRYERR_NOT_SUPPORTED;
		}

		ULONG pcbRes = 0;
		
		BCryptEncrypt(symko.mkHandle,
			(PUCHAR)in_src,
			in_length,
			NULL,
			NULL,
			0,
			NULL,
			0,
			&pcbRes,
			BCRYPT_BLOCK_PADDING
		);

		if (!pcbRes)
		{
			return crypto_error::MBASE_CRYERR_INTERNAL_ERROR;
		}

		ULONG outByteLength = pcbRes;
		PUCHAR outBytes = new UCHAR[outByteLength];

		NTSTATUS encryptResult = BCryptEncrypt(symko.mkHandle,
			(PUCHAR)in_src,
			in_length,
			NULL,
			NULL,
			0,
			outBytes,
			outByteLength,
			&pcbRes,
			BCRYPT_BLOCK_PADDING
		);

		if (encryptResult)
		{
			delete[] outBytes;
			return crypto_error::MBASE_CRYERR_INVALID_CALL;
		}

		out_target->bfSource = (IBYTEBUFFER)outBytes;
		out_target->bfLength = outByteLength;

		return crypto_error::MBASE_CRYERR_SUCCESS;
	}

	USED_RETURN MBASE_INLINE crypto_error _decrypt(IBYTEBUFFER in_src, SIZE_T in_length, safe_buffer* out_target) const noexcept {
		U64 blockLength = ckm.get_block_length();
		if (!blockLength)
		{
			return crypto_error::MBASE_CRYERR_NOT_SUPPORTED;
		}

		ULONG pcbRes = 0;
		// PASS NULL TO CALCULATE SIZE
		BCryptDecrypt(symko.mkHandle,
			(PUCHAR)in_src,
			in_length,
			NULL,
			NULL,
			0,
			NULL,
			0,
			&pcbRes,
			BCRYPT_BLOCK_PADDING
		);

		if (!pcbRes)
		{
			return crypto_error::MBASE_CRYERR_INTERNAL_ERROR;
		}

		ULONG outByteLength = pcbRes;
		PUCHAR outBytes = new UCHAR[outByteLength];

		NTSTATUS encryptResult = BCryptDecrypt(symko.mkHandle,
			(PUCHAR)in_src,
			in_length,
			NULL,
			NULL,
			0,
			outBytes,
			outByteLength,
			&pcbRes,
			BCRYPT_BLOCK_PADDING
		);

		if (encryptResult)
		{
			delete[] outBytes;
			return crypto_error::MBASE_CRYERR_INVALID_CALL;
		}

		out_target->bfSource = (IBYTEBUFFER)outBytes;
		out_target->bfLength = outByteLength;

		return crypto_error::MBASE_CRYERR_SUCCESS;
	}

	crypto_key_manager ckm;
	crypto_key_manager::sym_key_out symko;
};

class crypto_symmetric_aes : public crypto_symmetric_base {
public:
	crypto_symmetric_aes(IBYTEBUFFER in_key, SIZE_T in_length) noexcept : crypto_symmetric_base(in_key, in_length, gCryptoManager.symm_AES){}
	crypto_symmetric_aes() noexcept : crypto_symmetric_base(gCryptoManager.symm_AES) {}
};

MBASE_STD_END

#endif //