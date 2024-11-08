#ifndef MBASE_INF_PROCESSOR_H
#define MBASE_INF_PROCESSOR_H

#include <mbase/common.h>
#include <mbase/vector.h>
#include <mbase/list.h>
#include <mbase/string.h>
#include <mbase/set.h>
#include <mbase/synchronization.h>
#include <mbase/behaviors.h>
#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_context_line.h>
#include <mbase/inference/inf_sampling.h>
#include <mbase/inference/inf_sampling_set.h>
#include <mbase/framework/logical_processing.h>
#include <mbase/pc/pc_diagnostics.h>
#include <mutex>
#include <llama.h>

MBASE_BEGIN

/* === UPDATE_T template for all kinds of processors === */

/*
	while(is_processor_running())
    {
        if(is_registered())
        {
            if(signal_destroying())
            {
                _destroy_context();
            }

			if(is_running())
			{
				// signal based processing goes here
			}
        }
        else
        {
            if(signal_initializing())
            {
                _initialize_context();
            }
        }
    }
*/


static const U32 gProcessorMinimumTokenCount = 32;

class InfClientTextToText;
class InfProgram;

class MBASE_API InfProcessorBase : public mbase::logical_processor {
public:
	using size_type = SIZE_T;

	enum class flags : U8 {
		INF_PROC_SUCCESS,
		INF_PROC_ERR_UNREGISTERED_PROCESSOR,
		INF_PROC_ERR_ALREADY_INITIALIZED,
		INF_PROC_ERR_ALREADY_PROCESSING,
		INF_PROC_ERR_BELONGS_TO_ANOTHER_PROCESSOR,
		INF_PROC_ERR_MODEL_IS_NOT_INITIALIZED,
		INF_PROC_ERR_TOKEN_LIMIT_IS_TOO_LOW,
		INF_PROC_ERR_INPUT_EXCEED_TOKEN_LIMIT,
		INF_PROC_ERR_UNABLE_TO_TOKENIZE_INPUT,
		INF_PROC_ERR_INPUT_IS_EMPTY,
		INF_PROC_ERR_MISSING_CLIENT,
		INF_PROC_ERR_SAMPLER_NAME_MISMATCH,
		INF_PROC_INFO_INITIALIZING,
		INF_PROC_INFO_DESTROYING,
		INF_PROC_INFO_HALTED,
		INF_PROC_INFO_CONTINUE,
		INF_PROC_INFO_NEED_UPDATE
	};

	enum class processor_type : U8 {
		TEXT_TO_TEXT,
		EMBEDDER
	};

	enum class finish_state {
		FINISHED,
		CONTINUE,
		ABANDONED,
		TOKEN_LIMIT_REACHED
	};

	InfProcessorBase();

	bool is_registered() const;
	bool is_running();
	bool signal_state_initializing() const;
	bool signal_state_destroying() const;
	bool signal_initializing() const;
	bool signal_destroying() const;

	U32 get_context_size();
	InfModelBase* get_processed_model();
	U32 get_inactivity_threshold();
	processor_signal& get_initialize_signal();
	processor_signal& get_destroy_signal();
	const mbase::string& get_context_identifier();
	processor_type get_processor_type();

	GENERIC halt();
	GENERIC resume();

protected:
	InfModelBase* mTargetModel_md_model;
	bool mIsRunning;
	bool mIsRegistered;
	processor_signal mInitializeSignal;
	processor_signal mDestroySignal;
	processor_type mProcessorType;
	U32 mContextLength;
	U32 mInactivityThreshold;
	mbase::string mContextIdentifier;
}; // TODO: speech-to-text(whisper.cpp), text-to-text(llama.cpp), text-to-speech<EXPERIMENTAL>(bark.cpp), embedder

// class MBASE_API InfTextToTextEmbedder : public InfProcessorBase {
//public:
//private:
// };

MBASE_END

#endif // !MBASE_INF_PROCESSOR_H
