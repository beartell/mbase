#ifndef MBASE_INF_SAMPLING_H
#define MBASE_INF_SAMPLING_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/string.h>
#include <llama.h>

MBASE_BEGIN

#define MBASE_INF_SAMPLING_ARGS \
llama_context* in_ctx, llama_token_data_array* in_array, const mbase::string& in_sampler_name

#define MBASE_INF_SAMPLING_ARGS_CONSTRUCT \
InfSamplingBase(in_ctx, in_array, in_sampler_name)

class MBASE_API InfSamplingBase {
public:
	InfSamplingBase(MBASE_INF_SAMPLING_ARGS);
	virtual GENERIC apply_sampling() = 0;

	mbase::string get_sampler_name();
	F32 get_common_float();

	GENERIC set_context(llama_context* in_context);
	GENERIC set_token_array(llama_token_data_array* in_array);
	GENERIC set_common_float(F32 in_common_float);

protected:
	F32 mCommonFloat; // the reason this one exists is that almost all samplers except softmax, at least have one floating number to process
	mbase::string mSamplerName;
	llama_context* mSelfContext;
	llama_token_data_array* mTokenArray;
};

class MBASE_API InfSamplingTemperature : public InfSamplingBase {
public:
	InfSamplingTemperature(llama_context* in_ctx, llama_token_data_array* in_array);

	F32 get_min_temp();
	F32 get_max_temp();
	F32 get_temp();

	GENERIC set_temperature(F32 in_temperature);
	GENERIC apply_sampling() override;
};

class MBASE_API InfSamplingTopK : public InfSamplingBase {
public:
	InfSamplingTopK(llama_context* in_ctx, llama_token_data_array* in_array);

	I32 get_k();

	GENERIC set_k(I32 in_k);
	GENERIC apply_sampling() override;
};

class MBASE_API InfSamplingTypicalP : public InfSamplingBase {
public:
	InfSamplingTypicalP(llama_context* in_ctx, llama_token_data_array* in_array);

	F32 get_typical_p();

	GENERIC set_typical_p(F32 in_p);
	GENERIC apply_sampling() override;
};

class MBASE_API InfSamplingTopP : public InfSamplingBase {
public:
	InfSamplingTopP(llama_context* in_ctx, llama_token_data_array* in_array);

	F32 get_top_p();

	GENERIC set_top_p(F32 in_p);
	GENERIC apply_sampling() override;

};

class MBASE_API InfSamplingMinP : public InfSamplingBase {
public:
	InfSamplingMinP(llama_context* in_ctx, llama_token_data_array* in_array);

	F32 get_min_p();

	GENERIC set_min_p(F32 in_p);
	GENERIC apply_sampling() override;

};

class MBASE_API InfSamplingTailFree : public InfSamplingBase {
public:
	InfSamplingTailFree(llama_context* in_ctx, llama_token_data_array* in_array);

	F32 get_z_value();

	GENERIC set_z_value(F32 in_z);
	GENERIC apply_sampling() override;
};

class MBASE_API InfSamplingSoftmax : public InfSamplingBase {
public:
	InfSamplingSoftmax(llama_context* in_ctx, llama_token_data_array* in_array);
	
	GENERIC apply_sampling() override;
};

MBASE_END

#endif // !MBASE_INF_SAMPLING_H
