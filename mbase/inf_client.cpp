#include <mbase/inference/inf_client.h>
#include <mbase/inference/inf_model.h>
#include <mbase/inference/inf_embedder.h>
#include <mbase/io_file.h>
#include <iostream>

MBASE_BEGIN

#define MBASE_INF_CLIENT_USUAL_CHECK \
if(is_unregistering())\
{\
	return flags::INF_CLIENT_ERR_UNREGISTERATION_IN_PROGRESS;\
}\
if(!is_registered())\
{\
	return flags::INF_CLIENT_ERR_NOT_REGISTERED;\
}\
if(is_processing() || is_data_set())\
{\
	return flags::INF_CLIENT_ERR_PROCESSING;\
}\

InfClientTextToText::InfClientTextToText():
	mBaseProcessor(NULL),
	mChatHistory(),
	mMessageIndexer(0)
{
}

InfClientTextToText::InfClientTextToText(const InfClientTextToText& in_rhs):
	mBaseProcessor(NULL)
{
	mChatHistory = in_rhs.mChatHistory;
	mMessageIndexer = in_rhs.mMessageIndexer;
}

InfClientTextToText::~InfClientTextToText()
{
	InfProcessorBase* baseProcessor = get_host_processor();
	if(baseProcessor)
	{
		if(baseProcessor->get_processor_type() == InfProcessorBase::processor_type::TEXT_TO_TEXT)
		{
			InfTextToTextProcessor* t2tProcessor = static_cast<InfTextToTextProcessor*>(baseProcessor);
			t2tProcessor->release_inference_client_stacked();
		}
		else
		{
			InfEmbedderProcessor* embedProcessor = static_cast<InfEmbedderProcessor*>(baseProcessor);
			embedProcessor->release_inference_client_stacked();
		}
		//t2tProcessor->release_inference_client_stacked();
	}
}

InfClientTextToText& InfClientTextToText::operator=(const InfClientTextToText& in_rhs)
{
	// TODO, IMPLEMENTED HERE
	// IT IS NOT COMPLETE
	mChatHistory = in_rhs.mChatHistory;
	mMessageIndexer = in_rhs.mMessageIndexer;
	return *this;
}

bool InfClientTextToText::is_registered() const
{
	return (mBaseProcessor != NULL);
}

bool InfClientTextToText::has_message(const U32& in_msg_id) const
{
	if(mChatHistory.find(in_msg_id) == mChatHistory.end())
	{
		return false;
	}

	return true;
}

InfProcessorBase* InfClientTextToText::get_host_processor()
{
	return mBaseProcessor;
}

InfClientTextToText::flags InfClientTextToText::get_message(const U32& in_msg_id, context_line& out_message)
{
	try
	{
		out_message = mChatHistory.at(in_msg_id);
	}
	catch (const std::exception& out_except)
	{
		return flags::INF_CLIENT_ERR_MSG_ID_MISMATCH;
	}
	return flags::INF_CLIENT_SUCCESS;
}

InfClientTextToText::flags InfClientTextToText::get_message_array(PTRU32 in_msg_ids, size_type in_id_count, mbase::vector<context_line>& out_messages)
{
	mbase::vector<context_line> outMsgs;
	for(size_type i = 0; i < in_id_count; ++i)
	{
		try
		{
			outMsgs.push_back(mChatHistory.at(*(in_msg_ids + i)));
		}
		catch (const std::exception& out_except)
		{
			return flags::INF_CLIENT_ERR_MSG_ID_MISMATCH;
		}
	}
	out_messages = std::move(outMsgs);
	return flags::INF_CLIENT_SUCCESS;
}

GENERIC InfClientTextToText::on_embedding_data(const F32* out_data, size_type out_size)
{

}

GENERIC InfClientTextToText::_on_register(InfProcessorBase* in_processor)
{
	mBaseProcessor = in_processor;
	on_register(mBaseProcessor);
}

GENERIC InfClientTextToText::_on_unregister()
{
	mBaseProcessor = NULL;
	on_unregister();
}

InfClientTextToText::flags InfClientTextToText::add_message(CBYTEBUFFER in_data, size_type in_size, context_role in_role, U32& out_message_id)
{
	if(!in_data || !in_size)
	{
		return flags::INF_CLIENT_ERR_MISSING_INPUT;
	}
	
	mMessageIndexer++;
	mbase::string tmpMessage(in_data, in_size);
	context_line tempContextLine{ in_role, tmpMessage, mMessageIndexer };
	mChatHistory.insert(mbase::pair<U32, context_line>(mMessageIndexer, tempContextLine));
	out_message_id = mMessageIndexer;
	
	return flags::INF_CLIENT_SUCCESS;
}

InfClientTextToText::flags InfClientTextToText::add_message(const mbase::string& in_data, context_role in_role, U32& out_message_id)
{
	return add_message(in_data.c_str(), in_data.size(), in_role, out_message_id);
}

InfClientTextToText::flags InfClientTextToText::add_message(const mbase::wstring& in_data, context_role in_role, U32& out_message_id)
{
	return add_message(mbase::to_utf8(in_data), in_role, out_message_id);
}

InfClientTextToText::flags InfClientTextToText::remove_messages(const mbase::vector<U32>& in_msg_ids)
{
	if (in_msg_ids.size())
	{
		for (auto& n : in_msg_ids)
		{
			mChatHistory.erase(n);
		}
	}
	else
	{
		mChatHistory.clear();
	}
	return flags::INF_CLIENT_SUCCESS;
}

GENERIC InfClientTextToText::clear_chat_history()
{
	mChatHistory.clear();
	mMessageIndexer = 0;
}

MBASE_END