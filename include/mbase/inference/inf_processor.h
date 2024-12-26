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
#include <mbase/framework/logical_processing.h>
#include <mbase/framework/object_watcher.h>

MBASE_BEGIN

/* === UPDATE_T template for all kinds of processors === */

/*
	if(is_registered())
    {
        if(signal_destroying())
        {
            _destroy_context();
        }

        if(is_running())
        {
			... 
            while(is_processor_running())
            {
				... // Processing
                if(...)
                {
                    ...
                }
                else if(!<processor_behavior>.mHaltOnWrite)
                {
                    mbase::sleep(15);
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        if(signal_initializing())
        {
            _initialize_context();
        }
    }
*/

static const U32 gProcessorMinimumTokenCount = 32;

class InfClientBase;
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
		INF_PROC_INFO_NEED_UPDATE,
		INF_PROC_INFO_INITIAL_INPUT_PROCESSING,
		INF_PROC_INFO_DECODING,
		INF_PROC_INFO_NOT_IMPLEMENTED
	};

	enum class finish_state {
		FINISHED,
		CONTINUE,
		ABANDONED,
		TOKEN_LIMIT_REACHED,
		FAILED_ABANDONED
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	InfProcessorBase() noexcept;
	virtual ~InfProcessorBase() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) bool is_registered() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_running() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_initializing() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_destroying() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_initializing() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_destroying() const noexcept;
	MBASE_ND(MBASE_OBS_IGNORE) bool has_client() const noexcept;

	MBASE_ND(MBASE_OBS_IGNORE) U32 get_context_size() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) InfModelBase* get_processed_model();
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) InfClientBase* get_assigned_client();
	MBASE_ND(MBASE_OBS_IGNORE) U32 get_inactivity_threshold();
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) processor_signal& get_initialize_signal();
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) processor_signal& get_destroy_signal();
	MBASE_ND(MBASE_OBS_IGNORE) const mbase::string& get_context_identifier() const;
	MBASE_ND(MBASE_OBS_IGNORE) inf_model_category get_model_category() const;
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== STATE-MODIFIER METHODS BEGIN ===== */
	GENERIC acquire_object_watcher(mbase::inf_processor_watcher<InfProcessorBase>* in_watcher);
	GENERIC release_object_watcher();
	GENERIC halt();
	GENERIC resume();
	GENERIC reset_base_signals();
	virtual flags set_inference_client(InfClientBase* in_client);
	GENERIC release_inference_client();
	GENERIC release_inference_client_stacked();
	virtual flags destroy();
	virtual flags destroy_sync();
	/* ===== STATE-MODIFIER METHODS END ===== */

protected:
	InfClientBase* mAssignedClient;
	InfModelBase* mTargetModel_md_model;
	bool mIsRunning;
	bool mIsRegistered;
	processor_signal mInitializeSignal;
	processor_signal mDestroySignal;
	inf_model_category mModelCategory;
	U32 mContextLength;
	U32 mInactivityThreshold;
	mbase::string mContextIdentifier;
	mbase::inf_processor_watcher<InfProcessorBase>* mTargetWatcher;
}; // TODO: speech-to-text(whisper.cpp), text-to-text(llama.cpp), text-to-speech<EXPERIMENTAL>(bark.cpp), embedder

MBASE_END

#endif // !MBASE_INF_PROCESSOR_H
