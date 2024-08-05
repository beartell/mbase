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

	message-description-key = ALPHA *64(VCHAR)
	message-description-value = *WSP 1*128(VCHAR)
	message-description = message-description-key ":" message-description-value LF

	maip-end = "END" LF
	message-data = *OCTET

	maip-protocol-message = maip-identification-line *message-description maip-end message-data
*/

/*
	ERROR CODES:

*/

static const U32 gMaipVersionMinLength = 7;

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
	maip_description_value(const maip_description_value& in_rhs)
	{
		mValType = in_rhs.mValType;
		if(*mStringValue != '\0')
		{
			maip_sequence_helper::fill(mStringValue, '\0', 128);
		}
		maip_sequence_helper::copy_bytes(mStringValue, in_rhs.mStringValue);
	}
	maip_description_value& operator=(const maip_description_value& in_rhs)
	{
		mValType = in_rhs.mValType;
		if (*mStringValue != '\0')
		{
			maip_sequence_helper::fill(mStringValue, '\0', 128);
		}
		maip_sequence_helper::copy_bytes(mStringValue, in_rhs.mStringValue);
		return *this;
	}

	maip_value_type mValType = maip_value_type::MAIP_VALTPYE_STRING;
	union {
		IBYTE mStringValue[128];
		I64 mIntValue;
		F64 mFloatValue;
		bool mBoolValue;
	};
};

struct maip_description_kval {
	IBYTE mKey[64] = {0};
	mbase::vector<maip_description_value> mValues;
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
					return -1;
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
				return -1;
			}
			versionMajor[versionMajorLength] = in_stream.getc();
			++processedBytes;
			in_stream.advance_safe();
		}

		if (!tempFoundDot)
		{
			// INVALID FORMAT
			in_stream.reverse_safe(processedBytes);
			return -1;
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
			// invalid shit
			in_stream.reverse_safe(processedBytes);
			return -1;
		}
		++processedBytes;
		in_stream.advance_safe();

		out_version.mVersionMajor = atoi(versionMajor);
		out_version.mVersionMinor = atoi(versionMinor);
	}
	catch (const std::exception& out_except)
	{
		// MEANS, BUFFER IS OUT OF BOUNDS
		in_stream.reverse_safe(processedBytes);
		return -1;
	}

	return 0;
}

MBASE_INLINE I32 parse_req_identification_line(mbase::char_stream& in_stream, maip_request_identification& out_identification) 
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
				// MEANS OP LENGTH IS TOO LONG
				in_stream.reverse_safe(processedBytes);
				return -1;
			}

			if(!mbase::string::is_alnum(in_stream.getc()))
			{
				// contains non alpha numeric
				in_stream.reverse_safe(processedBytes);
				return -2;
			}
			opType += in_stream.getc();
			++processedBytes;
			in_stream.advance_safe();
		}

		if(!opType.size())
		{
			// MISSING OP TYPE
			in_stream.reverse_safe(processedBytes);
			return -3;
		}
		++processedBytes;
		in_stream.advance_safe();

		mbase::string opString;
		opLengthMax = 64;

		for(I32 i = 0; in_stream.getc() != 32; i++)
		{
			if(i == opLengthMax)
			{
				// OP LENGTH IS TOO LONG
				in_stream.reverse_safe(processedBytes);
				return -5;
			}

			if (!mbase::string::is_print(in_stream.getc()))
			{
				// contains non alpha numeric
				in_stream.reverse_safe(processedBytes);
				return -6;
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
			return -7;
		}

		out_identification.mOpType = std::move(opType);
		out_identification.mOpString = std::move(opString);
	}
	catch (const std::exception& out_except)
	{
		// MEANS, BUFFER IS OUT OF BOUNDS
		in_stream.reverse_safe(processedBytes);
		return -1;
	}
	return 0;
}

MBASE_INLINE I32 parse_message_description(mbase::char_stream& in_stream, maip_description_kval& out_kval)
{
	I32 processedBytes = 0;
	try
	{
		I32 maxKeyLength = 63;
		mbase::char_stream keyBuffer(out_kval.mKey, 64);
		for(I32 i = 0; in_stream.getc() != ':'; i++)
		{
			if(i == maxKeyLength)
			{
				in_stream.reverse_safe(processedBytes);
				return -2;
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
			return -3;
		}

		I32 maxValueLength = 128;
		I32	countValueLength = 0;

		++processedBytes;
		in_stream.advance_safe();
		maip_description_value mdv;
		for(I32 i = 0; in_stream.getc() != '\n' && i != 8192; i++)
		{
			if(countValueLength == 128)
			{
				// value too long
				in_stream.reverse_safe(processedBytes);
				return -4;
			}

			if(in_stream.getc() == 32)
			{
				// IGNORE SP | WSP
				if(countValueLength)
				{
					// invalid format
					in_stream.reverse_safe(processedBytes);
					return -5;
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
					return -6;
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
		return -1;
	}
	return 0;
}

MBASE_INLINE I32 parse_end(mbase::char_stream& in_stream)
{
	I32 bfLength = in_stream.buffer_length() - in_stream.get_pos();
	if(bfLength < 4)
	{
		return -2;
	}

	if (maip_sequence_helper::compare_bytes(in_stream.get_bufferc(), "END\n", 4))
	{
		// INVALID ENDING
		return -1;
	}

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
