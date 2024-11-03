#ifndef MBASE_INF_SAMPLING_SET_H
#define MBASE_INF_SAMPLING_SET_H

#include <mbase/common.h>
#include <set>

MBASE_BEGIN

struct MBASE_API InfSamplingRepetition {
    U32 mPenaltyN;
    F32 mRepeatPenalty;
    F32 mPenaltyFrequency;
    F32 mPenaltyPresent;
    bool mPenaltyLinefeed;
    bool mPenaltyEos;
};

struct MBASE_API InfSamplingXTC {
	F32 mProbability;
	F32 mThreshold;
};

struct MBASE_API InfSamplingDRY {
	F32 mDryMultiplier;
	F32 mDryBase;
	I32 mDryAllowedLength;
	I32 mDryPenaltyLastN;
};

struct MBASE_API InfSamplingMirostatV2 {
	F32 mTau;
	F32 mEta;
};

struct MBASE_API InfSamplerDescription {
	enum class SAMPLER : U32 {
        REPETITION,
        DRY,
		TOP_K,
		TOP_P,
		MIN_P,
		TYPICAL_P,
		TEMP,
		XTC,
		MIROSTAT_V2
	};

	friend bool operator<(const InfSamplerDescription& in_lhs, const InfSamplerDescription& in_rhs)
	{
		U32 samplerValueLeft = (U32)in_lhs.mSamplerType;
		U32 samplerValueRight = (U32)in_rhs.mSamplerType;

		return samplerValueLeft < samplerValueRight;
	}

	SAMPLER mSamplerType;

	union {
		U32 mTopK;
		F32 mTopP;
		F32 mMinP;
		F32 mTypicalP;
		F32 mTemp;
        InfSamplingRepetition mRepetition;
		InfSamplingXTC mXtc;
		InfSamplingDRY mDry;
		InfSamplingMirostatV2 mMiroV2;
	};
};

using inf_sampling_set = std::set<InfSamplerDescription>;

MBASE_END

#endif // MBASE_INF_SAMPLING_SET_H