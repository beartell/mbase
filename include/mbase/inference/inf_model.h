#ifndef MBASE_INF_MODEL_H
#define MBASE_INF_MODEL_H

#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/list.h>
#include <mbase/vector.h>
#include <mbase/unordered_map.h>
#include <mbase/behaviors.h>
#include <mbase/thread.h>
#include <mbase/framework/logical_processing.h>
#include <mbase/framework/thread_pool.h>
#include <mbase/framework/timer_loop.h>
#include <mbase/inference/inf_common.h>
#include <llama.h>

MBASE_BEGIN

class InfModelBase;
class InfProcessorBase;

class MBASE_API InfModelBase : public mbase::logical_processor {
public:
	using size_type = SIZE_T;
	using watcher_type = mbase::inf_processor_watcher<InfProcessorBase>;
	using context_processor_list = mbase::list<watcher_type>;
	using iterator = typename context_processor_list::iterator;
	using const_iterator = typename context_processor_list::const_iterator;
	using reverse_iterator = typename context_processor_list::reverse_iterator;
	using const_reverse_iterator = typename context_processor_list::const_reverse_iterator;

	enum class init_fail_code : U8 {
		NOT_ENOUGH_MEMORY,
		MBASE_PARAMS_DONT_MATCH,
		PATH_NOT_FOUND,
		LLAMA_SYSTEM_ERROR,
		UNDEFINED
	};

	/* ===== BUILDER METHODS BEGIN ===== */
	InfModelBase() noexcept;
	virtual ~InfModelBase() noexcept;
	/* ===== BUILDER METHODS END ===== */

	/* ===== ITERATOR METHODS BEGIN ===== */
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) iterator begin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) iterator end() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_iterator begin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_iterator end() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_iterator cbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_iterator cend() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) reverse_iterator rbegin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) reverse_iterator rend() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_reverse_iterator crbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const_reverse_iterator crend() const noexcept;
	/* ===== ITERATOR METHODS END ===== */

	/* ===== OBSERVATION METHODS BEGIN ===== */
	MBASE_ND(MBASE_OBS_IGNORE) context_processor_list& get_registered_processors();
	MBASE_ND(MBASE_OBS_IGNORE) inf_model_category get_model_category() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_initialize_failed() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool is_initialized() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_initializing() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_state_destroying() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_initializing() const;
	MBASE_ND(MBASE_OBS_IGNORE) bool signal_destroying() const;
	GENERIC reset_base_signals();
	/* ===== OBSERVATION METHODS END ===== */

	/* ===== INTERFACE METHODS BEGIN =====*/
	virtual GENERIC on_initializing();
	virtual GENERIC on_destroying();
	virtual GENERIC on_initialize_fail(init_fail_code out_fail_code) = 0;
	virtual GENERIC on_initialize() = 0;
	virtual GENERIC on_destroy() = 0;
	/* ===== INTERFACE METHODS END =====*/

protected:
	inf_model_category mModelCategory;
	volatile bool mIsInitialized;
	processor_signal mInitializeSignal;
	processor_signal mDestroySignal;

	context_processor_list mRegisteredProcessors;
	mbase::mutex mProcessorListMutex;
	init_fail_code mInitFailCode;
	bool mIsInitFailed;
};

// class MBASE_API InfModelImageToText : public InfModelBase{ // possibly using llava
//	...
// }

// class MBASE_API InfModelSpeechToText : public InfModelBase{ // possibly using whisper.cpp
//	...
// }

// class MBASE_API InfModelTextToImage : public InfModelBase{ // possibly using stable_diffusion.cpp
//	...
// }

// class MBASE_API InfModelTextToSpeech : public InfModelBase{ // possibly using bark.cpp
//	...
// }

// class MBASE_API InfModelEmbedding : public InfModelBase{ // Implement soon
//	...
// }

MBASE_END

#endif // !MBASE_INF_MODEL_H
