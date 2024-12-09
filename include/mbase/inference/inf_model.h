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

	InfModelBase();
	virtual ~InfModelBase();

	iterator begin() noexcept;
	iterator end() noexcept;
	const_iterator begin() const noexcept;
	const_iterator end() const noexcept;
	const_iterator cbegin() const noexcept;
	const_iterator cend() const noexcept;
	reverse_iterator rbegin() noexcept;
	reverse_iterator rend() noexcept;
	const_reverse_iterator crbegin() const noexcept;
	const_reverse_iterator crend() const noexcept;

	context_processor_list& get_registered_processors();
	inf_model_category get_model_category() const;
	bool is_initialize_failed() const;
	bool is_initialized() const;
	bool signal_state_initializing() const;
	bool signal_state_destroying() const;
	bool signal_initializing() const;
	bool signal_destroying() const;
	GENERIC reset_base_signals();

	virtual GENERIC on_initialize_fail(init_fail_code out_fail_code) = 0;
	virtual GENERIC on_initialize() = 0;
	virtual GENERIC on_destroy() = 0;
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
