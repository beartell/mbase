#ifndef MBASE_OPENAI_ERRORS_H
#define MBASE_OPENAI_ERRORS_H

#include <mbase/json/json.h>
#include <cpp-httplib/httplib.h>

MBASE_BEGIN

mbase::string buildOpenaiError(
    const mbase::string& in_message,
    const mbase::string& in_type,
    const mbase::string& in_code)
{
    mbase::Json errorDescription;
    errorDescription["message"] = in_message;
    errorDescription["type"] = in_type;
    errorDescription["code"] = in_code;

    mbase::Json openaiError;
    openaiError["error"] = errorDescription;

    return openaiError.toString();
}

GENERIC sendOpenaiError(
    const httplib::Request& in_req, 
    httplib::Response& in_resp,
    const mbase::string& in_message,
    const mbase::string& in_type,
    const mbase::string& in_code
)
{
    mbase::string responseString = buildOpenaiError(in_message, in_type, in_code);
    in_resp.set_content(responseString.c_str(), responseString.size(), "application/json");
}

bool openaiAuthCheck(const httplib::Request& in_req, httplib::Response& in_resp, const mbase::string& in_server_key, mbase::string& out_provided_key)
{
    if(!in_server_key.size())
    {
        return true;
    }

    if(in_req.has_header("Authorization"))
    {
        std::string authToken = in_req.get_header_value("Authorization");
        mbase::string authTokenField(authToken.c_str(), authToken.size());
        mbase::vector<mbase::string> seperatedField;

        authTokenField.split(" ", seperatedField);
        if(seperatedField.size() != 2) // 'Bearer' and 'key'
        {
            return false;
        }

        mbase::string bearerString = seperatedField[0];
        out_provided_key = seperatedField[1];

        if(bearerString != "Bearer")
        {
            return false;
        }

        if(out_provided_key != in_server_key)
        {
            return false;
        }

        return true;
    }
    
    return false;
}

MBASE_END

#endif // MBASE_OPENAI_ERRORS_H