#ifndef MBASE_PC_STATE_H
#define MBASE_PC_STATE_H

#include <mbase/common.h>
#include <mbase/behaviors.h>
#include <mbase/string.h>
#include <mbase/unordered_map.h>

MBASE_BEGIN

#ifdef MBASE_PLATFORM_WINDOWS
static mbase::wstring gDefaultStateDirectory = L".\\";
#endif

#ifdef MBASE_PLATFORM_UNIX
static mbase::wstring gDefaultStateDirectory = L"./";
#endif // MBASE_PLATFORM_WINDOWS

struct MBASE_API PcStateFileHeader {
	using size_type = SIZE_T;

	IBYTE mFileMagic[4] = { 0x4D, 0x42, 0x53, 0x46 }; // mbsf -> stands for 'mbase state file'
	U32 mStateStructCount = 0;
	U16 mMbaseVersionMajor = 0;
	U16 mMbaseVersionMinor = 0;
	mbase::string mStateObjectName = "";

	size_type get_serialized_size() const noexcept;
	GENERIC serialize(char_stream& out_stream) const;
	static PcStateFileHeader deserialize(IBYTEBUFFER in_src, size_type in_length, SIZE_T& bytes_processed);
};

struct MBASE_API PcSerializedStateStruct {
	using size_type = SIZE_T;

	PcSerializedStateStruct();
	PcSerializedStateStruct(const PcSerializedStateStruct& in_rhs);
	PcSerializedStateStruct(PcSerializedStateStruct&& in_rhs);
	~PcSerializedStateStruct();

	PcSerializedStateStruct& operator=(const PcSerializedStateStruct& in_rhs);
	PcSerializedStateStruct& operator=(PcSerializedStateStruct&& in_rhs);

	size_type get_serialized_size() const noexcept;
	GENERIC serialize(char_stream& out_stream) const;
	static PcSerializedStateStruct deserialize(IBYTEBUFFER in_src, size_type in_length, SIZE_T& bytes_processed);

	mbase::string mStateKey = "";
	size_type mStateValueLength = 0;
	IBYTEBUFFER mStateValue = NULL;
};

class MBASE_API PcState : public mbase::non_copyable {
public:
	using key_val_map = mbase::unordered_map<mbase::string, PcSerializedStateStruct>;
	using size_type = SIZE_T;

	enum class flags : U8 {
		STATE_SUCCESS,
		STATE_WARN_STATE_FILE_MISSING,
		STATE_ERR_OBJECT_NOT_INITIALIZED,
		STATE_ERR_MISSING_KEY,
		STATE_ERR_MISSING_DATA,
		STATE_ERR_NOT_FOUND,
		STATE_ERR_UNABLE_TO_SERIALIZE_DATA
	};

	PcState();
	PcState(PcState&& in_rhs) noexcept;
	~PcState();

	PcState& operator=(PcState&& in_rhs);

	flags initialize(const mbase::string& in_object_name, const mbase::wstring& in_state_path = gDefaultStateDirectory);
	flags initialize_overwrite(const mbase::string& in_object_name, const mbase::wstring& in_state_path = gDefaultStateDirectory);
	template<typename T>
	flags set_state(const mbase::string& in_key, const T& in_value)
	{
		if (!is_state_object_initialized())
		{
			return flags::STATE_ERR_OBJECT_NOT_INITIALIZED;
		}

		if (!in_key.size())
		{
			return flags::STATE_ERR_MISSING_KEY;
		}

		size_type serializedSize = mbase::get_serialized_size(in_value);
		if (!serializedSize)
		{
			return flags::STATE_ERR_MISSING_DATA;
		}
		deep_char_stream dcs(serializedSize);

		try
		{
			mbase::serialize(in_value, dcs);
			PcSerializedStateStruct stateStruct;
			stateStruct.mStateKey = in_key;
			stateStruct.mStateValue = dcs.get_buffer();
			stateStruct.mStateValueLength = serializedSize;
			dcs.release_buffer();
			mKvMap[in_key] = std::move(stateStruct);
			mIsModified = true;
		}
		catch (const std::exception& out_except)
		{
			return flags::STATE_ERR_UNABLE_TO_SERIALIZE_DATA;
		}

		return flags::STATE_SUCCESS;
	}

	flags remove_state(const mbase::string& in_key);
	GENERIC update();
	template<typename T>
	flags get_state(const mbase::string& in_key, T& out_state)
	{
		if (!is_state_object_initialized())
		{
			return flags::STATE_ERR_OBJECT_NOT_INITIALIZED;
		}

		key_val_map::iterator It = mKvMap.find(in_key);
		if (It == mKvMap.end())
		{
			return flags::STATE_ERR_NOT_FOUND;
		}
		size_type bytesProcessed = 0;
		out_state = mbase::deserialize<T>(It->second.mStateValue, It->second.mStateValueLength, bytesProcessed);
		return flags::STATE_SUCCESS;
	}
	mbase::string get_object_name();
	mbase::string get_full_state_name();
	bool is_state_modified();
	bool is_state_object_initialized();

private:
	bool mIsInitialized;
	bool mIsModified;
	key_val_map mKvMap;
	mbase::string mObjectName;
	mbase::string mStateFileSuffix;
	mbase::string mFullStateName;
	U16 mVersionMajor;
	U16 mVersionMinor;
};

MBASE_END

#endif // !MBASE_PC_STATE_H
