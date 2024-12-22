#include <mbase/pc/pc_state.h>
#include <mbase/string.h>
#include <mbase/traits.h>
#include <mbase/io_file.h>
#include <mbase/filesystem.h>

MBASE_BEGIN

typename PcStateFileHeader::size_type PcStateFileHeader::get_serialized_size() const noexcept
{
	return sizeof(mFileMagic) + sizeof(mStateStructCount) + sizeof(mMbaseVersionMajor) + sizeof(mMbaseVersionMinor) + mStateObjectName.get_serialized_size();
}

GENERIC PcStateFileHeader::serialize(char_stream& out_stream) const
{
	size_type headerTotalSize = this->get_serialized_size();
	if (out_stream.buffer_length() < headerTotalSize)
	{
		// do not serialize
		throw invalid_size();
	}
	
	out_stream.put_buffern(reinterpret_cast<CBYTEBUFFER>(mFileMagic), sizeof(mFileMagic));
	mbase::serialize(mStateStructCount, out_stream);
	mbase::serialize(mMbaseVersionMajor, out_stream);
	mbase::serialize(mMbaseVersionMinor, out_stream);
	mbase::serialize(mStateObjectName, out_stream);
}

PcStateFileHeader PcStateFileHeader::deserialize(IBYTEBUFFER in_src, size_type in_length, SIZE_T& bytes_processed)
{
	// TODO: if the length is invalid, handle that scenario
	mbase::char_stream tmpCharStream(in_src, in_length);
	IBYTE fileMagic[4] = { 0x4D, 0x42, 0x53, 0x46 };
	if (in_length < sizeof(fileMagic))
	{
		return PcStateFileHeader();
	}

	try
	{
		bytes_processed += sizeof(fileMagic);
		IBYTEBUFFER myFileMagic = tmpCharStream.get_buffer();
		tmpCharStream.advance_safe(sizeof(fileMagic));
		PcStateFileHeader fileHeader;
		if (!mbase::type_sequence<IBYTE>::compare_bytes(myFileMagic, fileMagic, sizeof(fileMagic)))
		{
			// good to go
			U32 stateStructCount = tmpCharStream.get_datan_safe<U32>();
			U16 mbaseVersionMajor = tmpCharStream.get_datan_safe<U16>();
			U16 mbaseVersionMinor = tmpCharStream.get_datan_safe<U16>();
			bytes_processed += sizeof(U32) + sizeof(U16) + sizeof(U16);
			size_type stringBytesProcessed = 0;
			mbase::string objectName = mbase::string::deserialize(tmpCharStream.get_bufferc(), tmpCharStream.get_difference(), stringBytesProcessed);
			bytes_processed += stringBytesProcessed;
			fileHeader.mMbaseVersionMajor = mbaseVersionMajor;
			fileHeader.mMbaseVersionMinor = mbaseVersionMinor;
			fileHeader.mStateStructCount = stateStructCount;
			fileHeader.mStateObjectName = std::move(objectName);
			tmpCharStream.advance(stringBytesProcessed);
		}

		return fileHeader;
	}
	catch ([[maybe_unused]] const std::exception& out_except)
	{
		// if the buffer is out of bounds
		return PcStateFileHeader();
	}
}

PcSerializedStateStruct::PcSerializedStateStruct() :
	mStateKey(""),
	mStateValueLength(0),
	mStateValue(NULL)
{
}

PcSerializedStateStruct::PcSerializedStateStruct(const PcSerializedStateStruct& in_rhs)
{
	mStateKey = in_rhs.mStateKey;
	mStateValueLength = in_rhs.mStateValueLength;

	if (mStateValueLength)
	{
		mStateValue = new IBYTE[mStateValueLength];
		mbase::type_sequence<IBYTE>::copy_bytes(mStateValue, in_rhs.mStateValue, mStateValueLength);
	}
}

PcSerializedStateStruct::PcSerializedStateStruct(PcSerializedStateStruct&& in_rhs)
{
	mStateKey = std::move(in_rhs.mStateKey);
	mStateValueLength = in_rhs.mStateValueLength;
	mStateValue = in_rhs.mStateValue;

	in_rhs.mStateValueLength = 0;
	in_rhs.mStateValue = NULL;
}

PcSerializedStateStruct::~PcSerializedStateStruct()
{
	if (mStateValue)
	{
		// IT SURELY IS HEAP ALLOCATED
		delete[] mStateValue;
	}
}

PcSerializedStateStruct& PcSerializedStateStruct::operator=(const PcSerializedStateStruct& in_rhs)
{
	if (mStateValue)
	{
		delete[] mStateValue;
	}

	mStateKey = in_rhs.mStateKey;
	mStateValueLength = in_rhs.mStateValueLength;
	if (mStateValueLength)
	{
		mStateValue = new IBYTE[mStateValueLength];
		mbase::type_sequence<IBYTE>::copy_bytes(mStateValue, in_rhs.mStateValue, mStateValueLength);
	}

	return *this;
}

PcSerializedStateStruct& PcSerializedStateStruct::operator=(PcSerializedStateStruct&& in_rhs)
{
	if (mStateValue)
	{
		delete[] mStateValue;
	}

	mStateKey = std::move(in_rhs.mStateKey);
	mStateValueLength = in_rhs.mStateValueLength;
	mStateValue = in_rhs.mStateValue;

	in_rhs.mStateValueLength = 0;
	in_rhs.mStateValue = NULL;

	return *this;
}

typename PcSerializedStateStruct::size_type PcSerializedStateStruct::get_serialized_size() const noexcept
{
	size_type keyBytesLength = mStateKey.get_serialized_size();
	size_type valueBytesLength = sizeof(size_type);
	return keyBytesLength + valueBytesLength + mStateValueLength;
}

GENERIC PcSerializedStateStruct::serialize(char_stream& out_stream) const
{
	if (out_stream.buffer_length() < get_serialized_size())
	{
		// do not serialize, buffer length is not enough
		throw invalid_size();
	}

	mStateKey.serialize(out_stream);
	out_stream.put_buffern(reinterpret_cast<CBYTEBUFFER>(&mStateValueLength), sizeof(size_type));
	out_stream.put_buffern(reinterpret_cast<CBYTEBUFFER>(mStateValue), mStateValueLength);
}

PcSerializedStateStruct PcSerializedStateStruct::deserialize(IBYTEBUFFER in_src, size_type in_length, SIZE_T& bytes_processed)
{
	char_stream tmpCharStream(in_src, in_length);
	size_type tmpBytesProcessed = 0;
	mbase::string stateKey = mbase::string::deserialize(tmpCharStream.get_buffer(), tmpCharStream.get_difference(), tmpBytesProcessed);
	IBYTEBUFFER stateValue = NULL;
	try
	{
		tmpCharStream.advance_safe(tmpBytesProcessed);
		size_type stateValueLength = tmpCharStream.get_datan_safe<size_type>();
		tmpBytesProcessed += sizeof(size_type);
		tmpBytesProcessed += stateValueLength;
		PcSerializedStateStruct outStruct;
		if (stateValueLength)
		{
			stateValue = new IBYTE[stateValueLength];
			CBYTEBUFFER newValues = tmpCharStream.get_bufferc();
			for (size_type i = 0; i < stateValueLength; ++i)
			{
				stateValue[i] = newValues[i];
			}

			outStruct.mStateKey = stateKey;
			outStruct.mStateValueLength = stateValueLength;
			outStruct.mStateValue = stateValue;
		}
		bytes_processed = tmpBytesProcessed;
		return outStruct;
	}

	catch ([[maybe_unused]] const std::exception& out_except)
	{
		if (stateValue)
		{
			delete[] stateValue;
		}
		return PcSerializedStateStruct();
	}
}

PcState::PcState() :
	mIsInitialized(false),
	mIsModified(false),
	mKvMap(),
	mObjectName(),
	mStateFileSuffix(),
	mFullStateName(),
	mVersionMajor(0),
	mVersionMinor(0)
{
}

PcState::PcState(PcState&& in_rhs) noexcept :
	mIsInitialized(in_rhs.mIsInitialized),
	mIsModified(in_rhs.mIsModified),
	mKvMap(std::move(in_rhs.mKvMap)),
	mObjectName(std::move(in_rhs.mObjectName)),
	mStateFileSuffix(std::move(in_rhs.mStateFileSuffix)),
	mFullStateName(std::move(in_rhs.mFullStateName)),
	mVersionMajor(in_rhs.mVersionMajor),
	mVersionMinor(in_rhs.mVersionMinor)
{
	in_rhs.mIsModified = false;
	in_rhs.mIsInitialized = false;

	in_rhs.mVersionMajor = 0;
	in_rhs.mVersionMinor = 0;
}

PcState::~PcState()
{
	// one last update before destruction
	update();
}

PcState& PcState::operator=(PcState&& in_rhs)
{
	mFullStateName = in_rhs.mFullStateName;
	mStateFileSuffix = in_rhs.mStateFileSuffix;
	mObjectName = in_rhs.mObjectName;
	mKvMap = std::move(in_rhs.mKvMap);
	mIsModified = in_rhs.mIsModified;
	mIsInitialized = in_rhs.mIsInitialized;
	mVersionMajor = in_rhs.mVersionMajor;
	mVersionMinor = in_rhs.mVersionMinor;

	in_rhs.mIsInitialized = false;
	in_rhs.mIsModified = false;
	return *this;
}

PcState::flags PcState::initialize(const mbase::string& in_object_name, const mbase::wstring& in_state_path)
{
	if (is_state_object_initialized())
	{
		if (mFullStateName == in_object_name)
		{
			return flags::STATE_SUCCESS;
		}
		update();
		mIsInitialized = false;
		mIsModified = false;
		mKvMap.clear();
	}

	mbase::wstring statePath = in_state_path;
	if(!statePath.size())
	{
		statePath = gDefaultStateDirectory;
	}

	#ifdef MBASE_PLATFORM_UNIX
	if(statePath.back() != L'/')
	{
		statePath += L'/';
	}
	#endif
	#ifdef MBASE_PLATFORM_WINDOWS
	if(statePath.back() != L'\\')
	{
		statePath += L'\\';
	}
	#endif

	if (in_object_name.size())
	{
		mFullStateName = mbase::to_utf8(statePath) + in_object_name;
		mObjectName = in_object_name;
		mbase::io_file ioStateFile;
		
		if(mbase::string::get_extension(mFullStateName) != "mbsf")
		{
			mFullStateName += ".mbsf";
		}
		
		ioStateFile.open_file(mbase::from_utf8(mFullStateName), mbase::io_file::access_mode::READ_ACCESS, mbase::io_file::disposition::APPEND);

		mIsInitialized = true; // FOR NOW, IT WILL WE MARKED INITIALIZED REGARDLESS OF ALL THE PROBLEMS
		if (ioStateFile.is_file_open())
		{
			ioStateFile.set_file_pointer(0, mbase::io_base::move_method::MV_BEGIN);
			size_type stateFileSize = ioStateFile.get_file_size();
			if (stateFileSize)
			{
				deep_char_stream dcs(stateFileSize);
				ioStateFile.read_data(dcs);
				dcs.set_cursor_front();
				size_type fileHeaderSize = 0;
				PcStateFileHeader stateFileHeader = PcStateFileHeader::deserialize(dcs.get_buffer(), dcs.buffer_length(), fileHeaderSize);

				// CHECK IF THE FILE MAGIC IS VALID
				// CHECK IF THE MBASE VERSION IS VALID
				mVersionMajor = stateFileHeader.mMbaseVersionMajor;
				mVersionMinor = stateFileHeader.mMbaseVersionMinor;
				try
				{
					dcs.advance_safe(fileHeaderSize);
				}
				catch ([[maybe_unused]] const std::exception& out_except)
				{

				}
				for (size_type i = 0; i < stateFileHeader.mStateStructCount; ++i)
				{
					size_type stateStructSize = 0;
					if (!dcs.get_difference())
					{
						break;
					}
					PcSerializedStateStruct stateStruct = PcSerializedStateStruct::deserialize(dcs.get_bufferc(), dcs.get_difference(), stateStructSize);
					mKvMap[stateStruct.mStateKey] = std::move(stateStruct);
					try
					{
						dcs.advance_safe(stateStructSize);
					}
					catch ([[maybe_unused]] const std::exception& out_except)
					{

					}
				}
			}
		}
		else
		{
			return flags::STATE_WARN_STATE_FILE_MISSING;
		}
	}
	return flags::STATE_SUCCESS;
}

PcState::flags PcState::initialize_overwrite(const mbase::string& in_object_name, const mbase::wstring& in_state_path)
{
	if (is_state_object_initialized())
	{
		if (mFullStateName == in_object_name)
		{
			return flags::STATE_SUCCESS;
		}
		update();
		mIsInitialized = false;
		mIsModified = false;
		mKvMap.clear();
	}

	mbase::wstring statePath = in_state_path;
	if (!statePath.size())
	{
		statePath = gDefaultStateDirectory;
	}
	#ifdef MBASE_PLATFORM_UNIX
	if(statePath.back() != L'/')
	{
		statePath += L'/';
	}
	#endif
	#ifdef MBASE_PLATFORM_WINDOWS
	if(statePath.back() != L'\\')
	{
		statePath += L'\\';
	}
	#endif
	if (in_object_name.size())
	{
		mFullStateName = mbase::to_utf8(statePath) + in_object_name;		
		if(mbase::string::get_extension(mFullStateName) != "mbsf")
		{
			mFullStateName += ".mbsf";
		}
				
		mObjectName = in_object_name;
		mbase::io_file ioStateFile;
		ioStateFile.open_file(mbase::from_utf8(mFullStateName));
		mIsInitialized = true; // FOR NOW, IT WILL WE MARKED INITIALIZED REGARDLESS OF ALL THE PROBLEMS
	}

	return flags::STATE_SUCCESS;
}

PcState::flags PcState::remove_state(const mbase::string& in_key)
{
	if (!is_state_object_initialized())
	{
		return flags::STATE_ERR_OBJECT_NOT_INITIALIZED;
	}

	if (!in_key.size())
	{
		return flags::STATE_ERR_MISSING_KEY;
	}

	key_val_map::iterator It = this->mKvMap.find(in_key);
	if (It == this->mKvMap.end())
	{
		return flags::STATE_ERR_NOT_FOUND;
	}

	this->mKvMap.erase(It);
	mIsModified = true;

	return flags::STATE_SUCCESS;
}

GENERIC PcState::update()
{
	if (!is_state_object_initialized())
	{
		return;
	}

	if (is_state_modified())
	{
		mbase::io_file iof;
		iof.open_file(mbase::from_utf8(mFullStateName));
		if (iof.is_file_open())
		{
			// write the state map into the .mbsf file
			PcStateFileHeader stateFileHeader;
			stateFileHeader.mMbaseVersionMajor = mVersionMajor;
			stateFileHeader.mMbaseVersionMinor = mVersionMinor;
			stateFileHeader.mStateObjectName = mObjectName;
			stateFileHeader.mStateStructCount = mKvMap.size();
			size_type serializedHeaderSize = stateFileHeader.get_serialized_size();
			deep_char_stream dcs(serializedHeaderSize);
			stateFileHeader.serialize(dcs);
			iof.write_data(dcs.get_buffer(), dcs.buffer_length());
			for (key_val_map::iterator It = mKvMap.begin(); It != mKvMap.end(); ++It)
			{
				size_type serializedStructSize = It->second.get_serialized_size();
				if (serializedStructSize)
				{
					dcs = std::move(deep_char_stream(serializedStructSize));
					It->second.serialize(dcs);
					iof.write_data(dcs.get_buffer(), dcs.buffer_length());
				}
			}
			mIsModified = false;
		}
	}
}

mbase::string PcState::get_object_name()
{
	return mObjectName;
}

mbase::string PcState::get_full_state_name()
{
	return mFullStateName;
}

bool PcState::is_state_modified()
{
	return mIsModified;
}

bool PcState::is_state_object_initialized()
{
	return mIsInitialized;
}

MBASE_END