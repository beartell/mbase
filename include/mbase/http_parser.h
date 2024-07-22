#ifndef MBASE_HTTP_PARSER
#define MBASE_HTTP_PARSER

#include <mbase/common.h>
#include <mbase/traits.h>
#include <mbase/string.h>
#include <mbase/unordered_map.h>
#include <mbase/vector.h>
#include <iostream>

MBASE_STD_BEGIN

/*
	Essentials ..

	Content-Encoding = gzip, compress, deflate, x-gzip, br, zstd
	Accept-Ranges = bytes
	Allow = GET, HEAD
	Content-Length = integer
	Content-Type = text/html, text/javascript, text/plain, application/json
	Server = mb-http
*/

static const mbase::pair<I32, MSTRING> gHttpResponseList[] = {
	{200, "OK"},
	{201, "CREATED"},
	{202, "Acepted"},
	{203, "Partial Information"},
	{204, "No Response"},
	{400, "Bad request"},
	{401, "Unauthorized"},
	{402, "PaymentRequired"},
	{403, "Forbidden"},
	{404, "Not found"},
	{500, "Internal Error"},
	{501, "Not implemented"},
	{502, "Service temporarily overloaded"},
	{503, "Gateway timeout"},
	{301, "Moved"},
	{302, "Found"},
	{303, "Method"},
	{304, "Not Modified"}
};

static const char gHttpContentTypes[] = {
	"text/html",
	"text/javascript",
	"text/plain",
	"application/json"
};

enum class http_content_type : U8 {
	TEXT_HTML,
	TEXT_JAVASCRIPT,
	TEXT_PLAIN,
	APP_JSON
};

enum class http_response_code : U8 {
	OK,
	CREATED,
	ACCEPTED,
	PARTIAL_INFORMATION,
	NO_RESPONSE,
	BAD_REQUEST,
	UNAUTHORIZED,
	PAYMENTREQUIRED,
	FORBIDDEN,
	NOT_FOUND,
	INTERNAL_ERROR,
	NOT_IMPLEMENTED,
	SERVICE_TEMPORARILY_OVERLOADED,
	GATEWAY_TIMEOUT,
	MOVED,
	FOUND,
	METHOD,
	NOT_MODIFIED
};

class http_response {
public:
	using size_type = SIZE_T;

	// request-line = method SP request-target SP HTTP-version CRLF
	// method = GET, POST, etc...
	// HTTP-version = HTTP/1.1

	// response-line = HTTP-version SP resp-code SP resp-text CRLF
	//  
	http_response();
	MBASE_INLINE GENERIC set_response_code(http_response_code in_code) noexcept;
	MBASE_INLINE GENERIC set_server(const mbase::string& in_name) noexcept;
	MBASE_INLINE GENERIC set_content_type(http_content_type in_type) noexcept;
	MBASE_INLINE GENERIC set_header_value(const mbase::string& in_name, const mbase::string& in_value) noexcept;
	MBASE_INLINE GENERIC generate_response(mbase::string& out_response, CBYTEBUFFER in_data = NULL, size_type in_size = 0) noexcept;

private:

	mbase::unordered_map<mbase::string, mbase::string> mFields;
};

MBASE_STD_END

#endif // !MBASE_HTTP_PARSER
