#ifndef MBASE_MAIP_PARSER_H
#define MBASE_MAIP_PARSER_H

#include <mbase/common.h>
#include <mbase/char_stream.h>
#include <mbase/string.h>
#include <mbase/unordered_map.h>
#include <ctype.h>

MBASE_STD_BEGIN

/*
	--- MAIP SYNTAX ---
	1000, 2000, 3000 ----> GENERIC SUCCESS, INF SUCCESS, EXEC SUCCESS

	maip-proto-major = 1*4DIGIT
	maip-proto-minor = 1*4DIGIT
	maip-proto-version-number = maip-proto-major "." maip-proto-minor
	maip-proto-name = "MAIP"
	maip-version = maip-proto-name maip-proto-version-number

	maip-req-op-type = "INF" / "EXEC" / 1*32(VCHAR)
	maip-req-op-name = 1*64(VCHAR)
	maip-req-op-string = maip-req-op-type SP maip-req-op-name
	maip-req-identification-line = maip-version SP maip-req-op-string

	maip-resp-status-code = 4*4DIGIT
	maip-resp-identification-line  = maip-version SP maip-resp-status-code

	maip-identification-line = maip-req-identification-line / maip-resp-identification-line SP LF

	message-description-key = ALPHA *64(VCHAR)
	message-description-value = *WSP 1*256(VCHAR)
	message-description = message-description-key ":" message-description-value LF

	maip-end = "END" LF
	message-data = *OCTET

	maip-protocol-message = maip-identification-line *message-description maip-end message-data
*/

enum class maip_generic_errors : U16 {
	SUCCESS = 1000,
	UNDEFINED_OP_TYPE,
	UNDEFINED_OP_STRING,
	OP_TYPE_TOO_LONG,
	OP_TYPE_NON_ALPHA,
	OP_STRING_TOO_LONG,
	OP_STRING_NON_PRINTABLE,
	OP_TIMED_OUT,
	INVALID_IDENTIFICATION_ENDING,
	INVALID_VERSION_MAJOR,
	INVALID_PROTOCOL,
	DATA_LENGTH_INCONSISTENCY,
	MISSING_MANDATORY_KEYS,
	MISSING_OP_TYPE,
	MISSING_KEY,
	MISSING_DATA,
	PACKET_TOO_LARGE,
	PACKET_TOO_SHORT,
	PACKET_INCOMPLETE,
	KEY_LENGTH_TOO_LARGE,
	VALUE_LENGTH_TOO_LARGE,
	INVALID_KVAL_FORMAT,
	ENGINE_OVERLOADED
};

static const U32 gMaipVersionMinLength = 7;
static const U32 gMaipDescriptionKeyLength = 64;
static const U32 gMaipDescriptionValueLength = 256;
static const U32 gMaipOpTypeLength = 32;
static const U32 gMaipOpStringLength = 64;
static const U32 gMaipMaxKvalCount = 64;

using maip_sequence_helper = mbase::type_sequence<IBYTE>;

enum class maip_value_type : U8 {
	MAIP_VALTPYE_STRING,
	MAIP_VALTYPE_INT,
	MAIP_VALTYPE_FLOAT
};

struct maip_version {
	I16 mVersionMajor = 0;
	I16 mVersionMinor = 0;
};

struct maip_request_identification {
	maip_version mVersion;
	mbase::string mOpType = "";
	mbase::string mOpString = "";
};

struct maip_response_identification {
	maip_version mVersion;
	I16 mResponseCode = 0;
};

struct maip_description_value {
	maip_description_value() : mStringValue{0} {}
	maip_description_value(const maip_description_value& in_rhs) : mStringValue{ 0 }
	{
		mValType = in_rhs.mValType;
		maip_sequence_helper::copy_bytes(mStringValue, in_rhs.mStringValue);
	}
	maip_description_value& operator=(const maip_description_value& in_rhs)
	{
		mValType = in_rhs.mValType;
		if (*mStringValue != '\0')
		{
			maip_sequence_helper::fill(mStringValue, '\0', gMaipDescriptionValueLength);
		}
		maip_sequence_helper::copy_bytes(mStringValue, in_rhs.mStringValue);
		return *this;
	}

	maip_value_type mValType = maip_value_type::MAIP_VALTPYE_STRING;
	union {
		IBYTE mStringValue[gMaipDescriptionValueLength];
		I64 mIntValue;
		F64 mFloatValue;
		bool mBoolValue;
	};
};

struct maip_description_kval {
	maip_description_kval() : mKey{0} {}
	maip_description_kval(const maip_description_kval& in_rhs) noexcept : mKey{0}
	{
		maip_sequence_helper::copy_bytes(mKey, in_rhs.mKey);
		mValues = in_rhs.mValues;
	}
	maip_description_kval(maip_description_kval&& in_rhs) noexcept : mKey{0}
	{
		maip_sequence_helper::copy_bytes(mKey, in_rhs.mKey);
		maip_sequence_helper::fill(in_rhs.mKey, '\0', gMaipDescriptionKeyLength);
		mValues = std::move(in_rhs.mValues);
	}

	maip_description_kval& operator=(const maip_description_kval& in_rhs)
	{
		maip_sequence_helper::fill(mKey, '\0', gMaipDescriptionKeyLength);
		maip_sequence_helper::copy_bytes(mKey, in_rhs.mKey);

		mValues = in_rhs.mValues;
		return *this;
	}

	maip_description_kval& operator=(maip_description_kval&& in_rhs) noexcept
	{
		maip_sequence_helper::fill(mKey, '\0', gMaipDescriptionKeyLength);
		maip_sequence_helper::copy_bytes(mKey, in_rhs.mKey);
		maip_sequence_helper::fill(in_rhs.mKey, '\0', gMaipDescriptionKeyLength);

		mValues = std::move(in_rhs.mValues);
		return *this;
	}

	IBYTE mKey[gMaipDescriptionKeyLength];
	mbase::vector<maip_description_value> mValues;
};

template<typename T>
struct to_string_ifnot_string {
	using pointer = const T*;
	pointer value;
	mbase::string get_value()
	{
		return mbase::to_string(*value);
	}
};

template<>
struct to_string_ifnot_string<mbase::string> {
	using pointer = const mbase::string*;
	pointer value;
	const mbase::string& get_value() 
	{
		return *value;
	}
};

// TODO: maip_packet_builder code is terrible here, come back later.
class maip_packet_builder {
public:
	enum class flags : U8 {
		PACKET_BUILDER_SUCCESS,
		PACKET_BUILDER_ERR_INVALID_OP_TYPE_FORMAT,
		PACKET_BUILDER_ERR_OP_TYPE_TOO_LONG,
		PACKET_BUILDER_ERR_INVALID_OP_STRING_FORMAT,
		PACKET_BUILDER_ERR_OP_STRING_TOO_LONG,
		PACKET_BUILDER_ERR_INVALID_RESPONSE_CODE_RANGE
	};

	using kval_container = mbase::unordered_map<mbase::string, mbase::vector<mbase::string>>;
	using size_type = SIZE_T;

	GENERIC set_version(U8 in_version_major, U8 in_version_minor);
	flags set_request_message(const mbase::string& in_op_type, const mbase::string& in_op_string);
	flags set_response_message(U16 in_status_code);
	template<typename Value>
	flags set_kval(const mbase::string& in_key, const Value& in_value);
	size_type generate_payload(mbase::string& out_payload);
	size_type generate_payload(mbase::string& out_payload, mbase::char_stream& in_stream);
	size_type generate_payload(mbase::string& out_payload, mbase::string in_string);

private:
	mbase::string mVersionString = "MAIP1.0 ";
	mbase::string mIdentificationLine = "1000 \n";
	kval_container mDescriptionKVals;
};

class maip_peer_request {
public:
	using kval_container = mbase::unordered_map<mbase::string, mbase::vector<maip_description_kval>>;
	using iterator = typename kval_container::iterator;
	using const_iterator = typename kval_container::const_iterator;

	maip_peer_request() noexcept : mData{0}, mDataStream(mData, 16384){};

	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE iterator begin() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE iterator end() noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator begin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator end() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator cbegin() const noexcept;
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE const_iterator cend() const noexcept;

	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const maip_version& get_version() const noexcept { return mMaipVersion; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) I16 get_version_major() const noexcept { return mMaipVersion.mVersionMajor; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) I16 get_version_minor() const noexcept { return mMaipVersion.mVersionMinor; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const maip_request_identification& get_identification() const noexcept { return mRequestIdentification; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) const mbase::unordered_map<mbase::string, mbase::vector<maip_description_kval>>& get_kvals() const noexcept { return mDescriptionKvals; }
	MBASE_ND(MBASE_IGNORE_NONTRIVIAL) U32 get_content_length() noexcept
	{
		mbase::string myString = "LENGTH";
		if(mDescriptionKvals.find("LENGTH") == mDescriptionKvals.end())
		{
			return 0;
		}
		mbase::vector<maip_description_value>& tempVals = mDescriptionKvals["LENGTH"].front().mValues;
		if(tempVals[0].mValType == maip_value_type::MAIP_VALTYPE_INT)
		{
			return mDescriptionKvals["LENGTH"].front().mValues[0].mIntValue;
		}
		return 0;
	}

	MBASE_INLINE maip_generic_errors parse_request(mbase::char_stream& in_stream);
	MBASE_INLINE maip_generic_errors parse_data(mbase::char_stream& in_stream, U32 in_length);

private:
	MBASE_INLINE maip_generic_errors _parse_version(mbase::char_stream& in_stream);
	MBASE_INLINE maip_generic_errors _parse_req_identification_line(mbase::char_stream& in_stream);
	MBASE_INLINE maip_generic_errors _parse_message_description(mbase::char_stream& in_stream, maip_description_kval& out_kval);
	MBASE_INLINE maip_generic_errors _parse_end(mbase::char_stream& in_stream);
	MBASE_INLINE maip_generic_errors _parse_message_data(mbase::char_stream& in_stream, IBYTEBUFFER* out_data, U64 out_size);

	MBASE_INLINE GENERIC _clear_request() 
	{
		mMaipVersion.mVersionMajor = 0;
		mMaipVersion.mVersionMinor = 0;
		mRequestIdentification = maip_request_identification();
		mDescriptionKvals.clear();
	}

	maip_version mMaipVersion;
	maip_request_identification mRequestIdentification;
	kval_container mDescriptionKvals;
	IBYTE mData[16384];
	mbase::char_stream mDataStream;
};

GENERIC maip_packet_builder::set_version(U8 in_version_major, U8 in_version_minor)
{
	mVersionString = mbase::string::from_format("MAIP%d.%d ", in_version_major, in_version_minor);
}

maip_packet_builder::flags maip_packet_builder::set_request_message(const mbase::string& in_op_type, const mbase::string& in_op_string)
{
	I32 lengthCounter = 0;
	mIdentificationLine = "";
	for (auto& n : in_op_type)
	{
		if (mbase::string::is_alpha(n))
		{
			if (lengthCounter == gMaipOpTypeLength)
			{
				mIdentificationLine = "1000 \n";
				return flags::PACKET_BUILDER_ERR_OP_TYPE_TOO_LONG;
			}
			++lengthCounter;
			mIdentificationLine.push_back(n);
		}
	}

	if (!mIdentificationLine.size())
	{
		mIdentificationLine = "1000 \n";
		return flags::PACKET_BUILDER_ERR_INVALID_OP_TYPE_FORMAT;
	}

	mIdentificationLine.push_back(' ');
	lengthCounter = 0;

	for (auto& n : in_op_string)
	{
		if (n == ' ')
		{
			continue;
		}

		if (mbase::string::is_print(n))
		{
			if (lengthCounter == gMaipOpStringLength)
			{
				mIdentificationLine = "1000 \n";
				return flags::PACKET_BUILDER_ERR_OP_STRING_TOO_LONG;
			}
			++lengthCounter;
			mIdentificationLine.push_back(n);
		}
	}

	if (!lengthCounter)
	{
		mIdentificationLine = "1000 \n";
		return flags::PACKET_BUILDER_ERR_INVALID_OP_STRING_FORMAT;
	}
	mIdentificationLine.push_back(' ');
	mIdentificationLine.push_back('\n');
	return flags::PACKET_BUILDER_SUCCESS;
}

maip_packet_builder::flags maip_packet_builder::set_response_message(U16 in_status_code)
{
	if (in_status_code < 1000 || in_status_code > 9999)
	{
		// MUST BE BETWEEN 1000-9999
		return flags::PACKET_BUILDER_ERR_INVALID_RESPONSE_CODE_RANGE;
	}
	mIdentificationLine = mbase::string::from_format("%d \n", in_status_code);
	return flags::PACKET_BUILDER_SUCCESS;
}

template<typename Value>
maip_packet_builder::flags maip_packet_builder::set_kval(const mbase::string& in_key, const Value& in_value)
{
	to_string_ifnot_string<Value> tss;
	tss.value = &in_value;

	mDescriptionKVals[in_key].push_back(tss.get_value());
	return flags::PACKET_BUILDER_SUCCESS;
}

typename maip_packet_builder::size_type maip_packet_builder::generate_payload(mbase::string& out_payload)
{
	mbase::string totalPayload = mVersionString + mIdentificationLine;
	for (auto& n : mDescriptionKVals)
	{
		totalPayload += n.first + ':';
		if (n.second.size() == 1)
		{
			totalPayload += n.second.front();
		}
		else
		{
			for (auto& m : n.second)
			{
				totalPayload += m + ';';
			}
			totalPayload.pop_back(); // remove the last semicolon
		}
		totalPayload += '\n';
	}
	totalPayload += "END\n";
	out_payload = std::move(totalPayload);
	return totalPayload.size(); // returns byte size
}

typename maip_packet_builder::size_type maip_packet_builder::generate_payload(mbase::string& out_payload, mbase::char_stream& in_stream)
{
	size_type streamLength = in_stream.buffer_length() - in_stream.get_pos();
	if (streamLength <= 0)
	{
		return generate_payload(out_payload); // returns byte size
	}
	set_kval("LENGTH", streamLength);
	mbase::string temporaryPayload;
	generate_payload(temporaryPayload);
	for (size_type i = 0; i < streamLength; i++)
	{
		temporaryPayload += in_stream.getc();
		in_stream.advance();
	}
	out_payload = std::move(temporaryPayload);
	return out_payload.size();
}

typename maip_packet_builder::size_type maip_packet_builder::generate_payload(mbase::string& out_payload, mbase::string in_string)
{
	mbase::char_stream cs(in_string.data(), in_string.size());
	return generate_payload(out_payload, cs);
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename maip_peer_request::iterator maip_peer_request::begin() noexcept
{
	return mDescriptionKvals.begin();
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename maip_peer_request::iterator maip_peer_request::end() noexcept
{
	return mDescriptionKvals.end();
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename maip_peer_request::const_iterator maip_peer_request::begin() const noexcept
{
	return mDescriptionKvals.begin();
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename maip_peer_request::const_iterator maip_peer_request::end() const noexcept
{
	return mDescriptionKvals.end();
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename maip_peer_request::const_iterator maip_peer_request::cbegin() const noexcept
{
	return mDescriptionKvals.cbegin();
}

MBASE_ND(MBASE_IGNORE_NONTRIVIAL) MBASE_INLINE typename maip_peer_request::const_iterator maip_peer_request::cend() const noexcept
{
	return mDescriptionKvals.cend();
}

MBASE_INLINE maip_generic_errors maip_peer_request::parse_request(mbase::char_stream& in_stream)
{
	_clear_request();

	maip_generic_errors parseResult = _parse_version(in_stream);
	if (parseResult != maip_generic_errors::SUCCESS)
	{
		return parseResult;
	}
	parseResult = _parse_req_identification_line(in_stream);
	if (parseResult != maip_generic_errors::SUCCESS)
	{
		return parseResult;
	}

	while (mDescriptionKvals.size() != gMaipMaxKvalCount) // max key values
	{
		maip_description_kval mdk;
		parseResult = _parse_message_description(in_stream, mdk);
		if (parseResult == maip_generic_errors::PACKET_INCOMPLETE)
		{
			break;
		}
		else if (parseResult == maip_generic_errors::SUCCESS)
		{
			mDescriptionKvals[mdk.mKey].push_back(std::move(mdk));
		}
		else
		{
			return parseResult;
		}
	}
	return _parse_end(in_stream);
}

MBASE_INLINE maip_generic_errors maip_peer_request::parse_data(mbase::char_stream& in_stream, U32 in_length)
{
	mDataStream.set_cursor_front();

	if(in_stream.get_pos() >= in_stream.buffer_length())
	{
		return maip_generic_errors::MISSING_DATA;
	}

	I32 processedBytes = 0;
	try
	{
		for(U32 i = 0; i < in_length; i++)
		{
			mDataStream.putc(in_stream.getc());
			++processedBytes;
			in_stream.advance_safe();
			mDataStream.advance_safe();
		}
	}
	catch (const std::exception& out_except)
	{
		in_stream.reverse_safe(processedBytes);
		return maip_generic_errors::DATA_LENGTH_INCONSISTENCY;
	}

	return maip_generic_errors::SUCCESS;
}

MBASE_INLINE maip_generic_errors maip_peer_request::_parse_version(mbase::char_stream& in_stream)
{
	if (in_stream.buffer_length() < gMaipVersionMinLength)
	{
		return maip_generic_errors::PACKET_TOO_SHORT;
	}

	if (maip_sequence_helper::compare_bytes(in_stream.get_buffer(), "MAIP", 4))
	{
		return maip_generic_errors::INVALID_PROTOCOL;
	}

	I32 processedBytes = 0;

	try
	{
		in_stream.advance_safe(4);

		I32 versionMajorLength = 0;
		I32 versionMinorLength = 0;
		IBYTE versionMajor[5] = { 0 };
		IBYTE versionMinor[5] = { 0 };
		bool tempFoundDot = false;

		for (versionMajorLength; versionMajorLength < 4; versionMajorLength++)
		{
			if (in_stream.getc() == '.')
			{
				if(!versionMajorLength)
				{
					// missing version major
					in_stream.reverse_safe(processedBytes);
					return maip_generic_errors::INVALID_VERSION_MAJOR;
				}
				// means version major is formed
				++processedBytes;
				in_stream.advance_safe();
				tempFoundDot = true;
				break;
			}

			if (!isdigit(in_stream.getc()))
			{
				// means version major contains non digit
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::INVALID_VERSION_MAJOR;
			}
			versionMajor[versionMajorLength] = in_stream.getc();
			++processedBytes;
			in_stream.advance_safe();
		}

		if (!tempFoundDot)
		{
			// INVALID FORMAT
			in_stream.reverse_safe(processedBytes);
			return maip_generic_errors::INVALID_IDENTIFICATION_ENDING;
		}

		for (versionMinorLength; versionMinorLength < 4; versionMinorLength++)
		{
			if (!isdigit(in_stream.getc()))
			{
				break;
			}
			
			versionMinor[versionMinorLength] = in_stream.getc();
			++processedBytes;
			in_stream.advance_safe();
		}

		if(!versionMinorLength)
		{
			in_stream.reverse_safe(processedBytes);
			return maip_generic_errors::INVALID_IDENTIFICATION_ENDING;
		}
		++processedBytes;
		in_stream.advance_safe();

		mMaipVersion.mVersionMajor = atoi(versionMajor);
		mMaipVersion.mVersionMinor = atoi(versionMinor);
	}
	catch (const std::exception& out_except)
	{
		// MEANS, BUFFER IS OUT OF BOUNDS
		in_stream.reverse_safe(processedBytes);
		return maip_generic_errors::PACKET_TOO_SHORT;
	}

	return maip_generic_errors::SUCCESS;
}

MBASE_INLINE maip_generic_errors maip_peer_request::_parse_req_identification_line(mbase::char_stream& in_stream)
{
	I32 processedBytes = 0;
	try
	{
		mbase::string opType;
		I32 opLengthMax = 32;
		for(I32 i = 0; in_stream.getc() != 32; i++)
		{
			if(i == opLengthMax)
			{
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::OP_TYPE_TOO_LONG;
			}

			if(!mbase::string::is_alnum(in_stream.getc()))
			{
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::OP_TYPE_NON_ALPHA;
			}
			opType += in_stream.getc();
			++processedBytes;
			in_stream.advance_safe();
		}

		if(!opType.size())
		{
			in_stream.reverse_safe(processedBytes);
			return maip_generic_errors::MISSING_OP_TYPE;
		}
		++processedBytes;
		in_stream.advance_safe();

		mbase::string opString;
		opLengthMax = gMaipOpStringLength;

		for(I32 i = 0; in_stream.getc() != 32; i++)
		{
			if(i == opLengthMax)
			{
				// OP LENGTH IS TOO LONG
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::OP_STRING_TOO_LONG;
			}

			if(in_stream.getc() == 32)
			{
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::OP_STRING_NON_PRINTABLE;
			}

			if (!mbase::string::is_print(in_stream.getc()))
			{
				// contains non alpha numeric
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::OP_STRING_NON_PRINTABLE;
			}

			opString += in_stream.getc();
			++processedBytes;
			in_stream.advance_safe();
		}
		++processedBytes;
		in_stream.advance_safe();
		if(in_stream.getc() != '\n')
		{
			// ENDING IS INVALID
			in_stream.reverse_safe(processedBytes);
			return maip_generic_errors::INVALID_IDENTIFICATION_ENDING;
		}
		++processedBytes;
		in_stream.advance_safe();
		mRequestIdentification.mOpType = std::move(opType);
		mRequestIdentification.mOpString = std::move(opString);
	}
	catch (const std::exception& out_except)
	{
		// MEANS, BUFFER IS OUT OF BOUNDS
		in_stream.reverse_safe(processedBytes);
		return maip_generic_errors::PACKET_TOO_SHORT;
	}
	return maip_generic_errors::SUCCESS;
}

MBASE_INLINE maip_generic_errors maip_peer_request::_parse_message_description(mbase::char_stream& in_stream, maip_description_kval& out_kval)
{
	I32 processedBytes = 0;
	try
	{
		I32 maxKeyLength = gMaipDescriptionKeyLength - 1;
		mbase::char_stream keyBuffer(out_kval.mKey, gMaipDescriptionKeyLength);
		for(I32 i = 0; in_stream.getc() != ':'; i++)
		{
			if(in_stream.getc() == '\n') 
			{
				// this is for understanding that we are reached the END LF
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::PACKET_INCOMPLETE;
			}

			if(i == maxKeyLength)
			{
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::KEY_LENGTH_TOO_LARGE;
			}
			keyBuffer.putc(in_stream.getc());
			++processedBytes;
			in_stream.advance_safe();
			keyBuffer.advance_safe();
		}

		if(!keyBuffer.get_pos())
		{
			// MISSING KEY
			in_stream.reverse_safe(processedBytes);
			return maip_generic_errors::MISSING_KEY;
		}

		I32 maxValueLength = gMaipDescriptionValueLength;
		I32	countValueLength = 0;

		++processedBytes;
		in_stream.advance_safe();
		maip_description_value mdv;
		for(I32 i = 0; in_stream.getc() != '\n' && i != 8192; i++)
		{
			if(countValueLength == gMaipDescriptionValueLength)
			{
				// value too long
				in_stream.reverse_safe(processedBytes);
				return maip_generic_errors::VALUE_LENGTH_TOO_LARGE;
			}

			if(in_stream.getc() == 32)
			{
				// IGNORE SP | WSP
				if(countValueLength)
				{
					// invalid format
					in_stream.reverse_safe(processedBytes);
					return maip_generic_errors::INVALID_KVAL_FORMAT;
				}

				++processedBytes;
				in_stream.advance_safe();
				continue;
			}
			else
			{
				if(!mbase::string::is_print(in_stream.getc()))
				{
					// non printable characters
					in_stream.reverse_safe(processedBytes);
					return maip_generic_errors::INVALID_KVAL_FORMAT;
				}
				
				if(in_stream.getc() == ';')
				{
					if(!countValueLength)
					{
						// do nothing
					}
					else
					{
						out_kval.mValues.push_back(mdv);
						maip_sequence_helper::fill(mdv.mStringValue, '\0', countValueLength);
						countValueLength = 0;
					}
				}
				else
				{
					mdv.mStringValue[countValueLength] = in_stream.getc();
					++countValueLength;
				}
			}
			++processedBytes;
			in_stream.advance_safe();
		}
		++processedBytes;
		in_stream.advance_safe();

		if(countValueLength)
		{
			out_kval.mValues.push_back(mdv);
		}

		for(auto &n : out_kval.mValues)
		{
			if(mbase::string::is_integer(n.mStringValue))
			{
				n.mValType = maip_value_type::MAIP_VALTYPE_INT;
				n.mIntValue = atoi(n.mStringValue);
			}
			else if(mbase::string::is_float(n.mStringValue))
			{
				n.mValType = maip_value_type::MAIP_VALTYPE_FLOAT;
				n.mFloatValue = atof(n.mStringValue);
			}
			else 
			{
				n.mValType = maip_value_type::MAIP_VALTPYE_STRING;
			}
		}
	}

	catch (const std::exception& out_except)
	{
		// MEANS, BUFFER IS OUT OF BOUNDS
		in_stream.reverse_safe(processedBytes);
		return maip_generic_errors::PACKET_TOO_SHORT;
	}
	return maip_generic_errors::SUCCESS;
}

MBASE_INLINE maip_generic_errors maip_peer_request::_parse_end(mbase::char_stream& in_stream)
{
	I32 bfLength = in_stream.buffer_length() - in_stream.get_pos();
	if(bfLength < 4)
	{
		return maip_generic_errors::PACKET_INCOMPLETE;
	}

	if (maip_sequence_helper::compare_bytes(in_stream.get_bufferc(), "END\n", 4))
	{
		// INVALID ENDING
		return maip_generic_errors::PACKET_INCOMPLETE;
	}

	return maip_generic_errors::SUCCESS;
}

MBASE_INLINE maip_generic_errors _parse_message_data(mbase::char_stream& in_stream, IBYTEBUFFER* out_data, U64 out_size)
{
	return maip_generic_errors::SUCCESS;
}

class MaipRequest {
public:

private:
};

MBASE_STD_END

#endif // !MBASE_MAIP_PARSER_H
