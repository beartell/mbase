#include <mbase/inference/inf_sampling.h>

MBASE_BEGIN

InfSamplingBase::InfSamplingBase(MBASE_INF_SAMPLING_ARGS) : mSelfContext(in_ctx), mTokenArray(in_array), mSamplerName(in_sampler_name), mCommonFloat(0.0f)
{
}

mbase::string InfSamplingBase::get_sampler_name()
{
	return mSamplerName;
}

F32 InfSamplingBase::get_common_float()
{
	return mCommonFloat;
}

GENERIC InfSamplingBase::set_context(llama_context* in_context)
{
	mSelfContext = in_context;
}

GENERIC InfSamplingBase::set_token_array(llama_token_data_array* in_array)
{
	mTokenArray = in_array;
}

GENERIC InfSamplingBase::set_common_float(F32 in_common_float)
{
	mCommonFloat = in_common_float;
}

InfSamplingTemperature::InfSamplingTemperature(llama_context* in_ctx, llama_token_data_array* in_array) : InfSamplingBase(in_ctx, in_array, "temp")
{
}

F32 InfSamplingTemperature::get_min_temp()
{
	return 0.0;
}

F32 InfSamplingTemperature::get_max_temp()
{
	return 1.4;
}

GENERIC InfSamplingTemperature::set_temperature(F32 in_temperature)
{
	if(in_temperature < get_min_temp())
	{
		mCommonFloat = get_min_temp();
	}

	if(in_temperature > get_max_temp())
	{
		mCommonFloat = get_max_temp();
	}
}

GENERIC InfSamplingTemperature::apply_sampling()
{
	printf("TEMP SAMPLING APPLIED WITH VALUE: %f\n", mCommonFloat);
	llama_sample_temp(mSelfContext, mTokenArray, mCommonFloat);
}

InfSamplingTopK::InfSamplingTopK(llama_context* in_ctx, llama_token_data_array* in_array) : InfSamplingBase(in_ctx, in_array, "top_k")
{
}

I32 InfSamplingTopK::get_k()
{
	return mCommonFloat;
}

GENERIC InfSamplingTopK::set_k(I32 in_k)
{
	mCommonFloat = in_k;
}

GENERIC InfSamplingTopK::apply_sampling()
{
	llama_sample_top_k(mSelfContext, mTokenArray, mCommonFloat, 0);
}

InfSamplingTypicalP::InfSamplingTypicalP(llama_context* in_ctx, llama_token_data_array* in_array) : InfSamplingBase(in_ctx, in_array, "typical_p")
{

}

F32 InfSamplingTypicalP::get_typical_p()
{
	return mCommonFloat;
}

GENERIC InfSamplingTypicalP::set_typical_p(F32 in_p)
{
	mCommonFloat = in_p;
}

GENERIC InfSamplingTypicalP::apply_sampling()
{
	llama_sample_typical(mSelfContext, mTokenArray, mCommonFloat, 0);
}

InfSamplingTopP::InfSamplingTopP(llama_context* in_ctx, llama_token_data_array* in_array) : InfSamplingBase(in_ctx, in_array, "top_p")
{
}

F32 InfSamplingTopP::get_top_p()
{
	return mCommonFloat;
}

GENERIC InfSamplingTopP::set_top_p(F32 in_p)
{
	mCommonFloat = in_p;
}

GENERIC InfSamplingTopP::apply_sampling()
{
	llama_sample_top_p(mSelfContext, mTokenArray, mCommonFloat, 0);
}

InfSamplingMinP::InfSamplingMinP(llama_context* in_ctx, llama_token_data_array* in_array) : InfSamplingBase(in_ctx, in_array, "min_p")
{

}

F32 InfSamplingMinP::get_min_p()
{
	return mCommonFloat;
}

GENERIC InfSamplingMinP::set_min_p(F32 in_p)
{
	mCommonFloat = in_p;
}

GENERIC InfSamplingMinP::apply_sampling() 
{
	llama_sample_min_p(mSelfContext, mTokenArray, mCommonFloat, 0);
}

InfSamplingTailFree::InfSamplingTailFree(llama_context* in_ctx, llama_token_data_array* in_array) : InfSamplingBase(in_ctx, in_array, "tailfree")
{
}

F32 InfSamplingTailFree::get_z_value()
{
	return mCommonFloat;
}

GENERIC InfSamplingTailFree::set_z_value(F32 in_z)
{
	mCommonFloat = in_z;
}

GENERIC InfSamplingTailFree::apply_sampling()
{
	llama_sample_tail_free(mSelfContext, mTokenArray, mCommonFloat, 0);
}

InfSamplingSoftmax::InfSamplingSoftmax(llama_context* in_ctx, llama_token_data_array* in_array) : InfSamplingBase(in_ctx, in_array, "softmax")
{

}

GENERIC InfSamplingSoftmax::apply_sampling()
{
	llama_sample_softmax(mSelfContext, mTokenArray);
}


MBASE_END