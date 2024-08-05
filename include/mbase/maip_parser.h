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

	maip-identification-line = maip-req-identification-line / maip-resp-identification-line LF

	message-description-key = ALPHA *128(VCHAR)
	message-description-value = 1*128(VCHAR) *WSP
	message-description = message-description-key ":" message-description-value LF

	maip-end = "END" LF
	message-data = *OCTET

	maip-protocol-message = maip-identification-line *message-description maip-end message-data
*/

static const U32 gMaipVersionMinLength = 7;

using maip_sequence_helper = mbase::type_sequence<IBYTE>;

enum class maip_value_type : U8 {
	MAIP_VALTPYE_STRING,
	MAIP_VALTYPE_INT,
	MAIP_VALTYPE_FLOAT,
	MAIP_VALTYPE_BOOL
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
	I8 mVersionMajor = 0;
	I8 mVersionMinor = 0;
	I16 mResponseCode = 0;
};

struct maip_description_value {
	maip_value_type mValType = maip_value_type::MAIP_VALTPYE_STRING;
	union {
		IBYTE mStringValue[128] = {0};
		I64 mIntValue;
		F64 mFloatValue;
		bool mBoolValue;
	};
};

struct maip_description_kval {
	IBYTE mKey[64] = {0};
	maip_description_value mValue;
};

MBASE_INLINE I32 parse_version(mbase::char_stream& in_stream, maip_version& out_version) 
{
	if (in_stream.buffer_length() < gMaipVersionMinLength)
	{
		// INVALID INPUT LENGTH
		return -1;
	}

	if (maip_sequence_helper::compare_bytes(in_stream.get_buffer(), "MAIP", 4))
	{
		// INVALID PROTOCOL
		return -1;
	}

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
			versionMajor[versionMajorLength] = in_stream.getc();
			if (!isdigit(versionMajor[versionMajorLength]))
			{
				// means version major contains non digit
				return -1;
			}

			in_stream.advance_safe();
			if (in_stream.getc() == '.')
			{
				// means version major is formed
				in_stream.advance_safe();
				tempFoundDot = true;
				break;
			}
		}

		if (!tempFoundDot)
		{
			// INVALID FORMAT
			return -1;
		}

		for (versionMinorLength; versionMinorLength < 4; versionMinorLength++)
		{
			if (!isdigit(in_stream.getc()))
			{
				break;
			}
			
			versionMinor[versionMinorLength] = in_stream.getc();
			in_stream.advance_safe();
		}

		if(!versionMinorLength)
		{
			// invalid shit
			return -1;
		}

		out_version.mVersionMajor = atoi(versionMajor);
		out_version.mVersionMinor = atoi(versionMinor);
	}
	catch (const std::exception& out_except)
	{
		// MEANS, BUFFER IS OUT OF BOUNDS
		return -1;
	}

	return 0;
}

MBASE_INLINE I32 parse_req_identification_line(mbase::char_stream& in_stream, maip_request_identification& out_identification) 
{
	return 0;
}

MBASE_INLINE I32 parse_resp_identification_line(mbase::char_stream& in_stream, maip_response_identification& out_identification)
{
	return 0;
}

MBASE_INLINE I32 parse_message_description(mbase::char_stream& in_stream, mbase::vector<maip_description_kval>& out_description)
{
	return 0;
}

MBASE_INLINE I32 parse_end(mbase::char_stream& in_stream)
{
	return 0;
}

MBASE_INLINE I32 parse_message_data(mbase::char_stream& in_stream, IBYTEBUFFER* out_data, U64 out_size)
{
	return 0;
}

class MaipRequest {
public:

private:
};

MBASE_STD_END

#endif // !MBASE_MAIP_PARSER_H
