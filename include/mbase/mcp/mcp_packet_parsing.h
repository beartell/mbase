#ifndef MBASE_MCP_PACKET_PARSING_H
#define MBASE_MCP_PACKET_PARSING_H

#include <mbase/mcp/mcp_common.h>

MBASE_BEGIN

inline bool validate_mcp_request_rpc2(mbase::Json& in_json)
{
    // From: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages ->
    // All messages in MCP MUST follow the JSON-RPC 2.0 specification.

    if(!in_json["jsonrpc"].isString() || !in_json["method"].isString())
    {
        // RPC MESSAGE FORMAT IS INCORRECT
        // Spec: https://www.jsonrpc.org/specification
        return false;
    }

    const mbase::string& rpcVersion = in_json["jsonrpc"].getString();

    if(rpcVersion != "2.0")
    {
        // From: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages ->
        // All messages in MCP MUST follow the JSON-RPC 2.0 specification.
        return false;
    }

    return true;
}

inline bool validate_mcp_response_rpc2(mbase::Json& in_json)
{
    // From: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages ->
    // All messages in MCP MUST follow the JSON-RPC 2.0 specification.

    if(!in_json["jsonrpc"].isString())
    {
        // RPC MESSAGE FORMAT IS INCORRECT
        // Spec: https://www.jsonrpc.org/specification
        return false;
    }

    const mbase::string& rpcVersion = in_json["jsonrpc"].getString();

    if(rpcVersion != "2.0")
    {
        // From: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages ->
        // All messages in MCP MUST follow the JSON-RPC 2.0 specification.
        return false;
    }

    return true;
}

// true if the packet is valid, false if not
// If true, json will be populated
inline bool validate_mcp_request_packet(const mbase::string& in_packet, mbase::Json& out_json)
{
    if(!in_packet.size())
    {
        // Packet must exist
        return false;
    }

    std::pair<mbase::Json::Status, mbase::Json> jsParseResult = mbase::Json::parse(in_packet);

    if(jsParseResult.first != mbase::Json::Status::success)
    {
        // Messaging is in JSON-RPC 2.0 format
        // Spec: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages
        return false;
    }

    mbase::Json& parsedJson = jsParseResult.second;
    if(parsedJson.isArray())
    {
        // JSON-RPC 2.0 batch process is supported
        for(mbase::Json& currentPacket : parsedJson.getArray())
        {
            if(!validate_mcp_request_rpc2(currentPacket))
            {
                return false;
            }
        }
    }
    else
    {
        if(!validate_mcp_request_rpc2(parsedJson))
        {
            return false;
        }
    }

    out_json = parsedJson;
    return true;
}

// true if the packet is valid, false if not
// If true, json will be populated
inline bool validate_mcp_response(const mbase::string& in_packet, mbase::Json& out_json)
{
    if(!in_packet.size())
    {
        // Packet must exist
        return false;
    }

    std::pair<mbase::Json::Status, mbase::Json> jsParseResult = mbase::Json::parse(in_packet);

    if(jsParseResult.first != mbase::Json::Status::success)
    {
        // Messaging is in JSON-RPC 2.0 format
        // Spec: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages
        return false;
    }

    mbase::Json& parsedJson = jsParseResult.second;
    if(parsedJson.isArray())
    {
        // JSON-RPC 2.0 batch process is supported
        for(mbase::Json& currentPacket : parsedJson.getArray())
        {
            if(!validate_mcp_response_rpc2(currentPacket))
            {
                return false;
            }
        }
    }
    else
    {
        if(!validate_mcp_response_rpc2(parsedJson))
        {
            return false;
        }
    }

    out_json = parsedJson;
    return true;
}

inline mbase::string mcp_generate_error_message(
    const mbase::Json& in_id,
    const mbase::I32& in_code,
    const mbase::string& in_message,
    const mbase::Json& in_data = mbase::Json() // optional
)
{
    // Spec: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages#responses
    mbase::Json responsePacket;
    responsePacket["jsonrpc"] = "2.0";
    responsePacket["id"] = in_id;
    responsePacket["error"]["code"] = in_code;
    responsePacket["error"]["message"] = in_message;
    responsePacket["error"]["data"] = in_data;

    return responsePacket.toString() + '\n';
}

inline mbase::string mcp_generate_response(
    const mbase::Json& in_id,
    const mbase::Json& in_response = mbase::Json()
)
{
    // Spec: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages#responses
    mbase::Json responsePacket;
    responsePacket["jsonrpc"] = "2.0";
    responsePacket["id"] = in_id;
    responsePacket["result"].setObject();
    if(in_response.isObject())
    {
        responsePacket["result"] = in_response;
    }

    return responsePacket.toString() + '\n';
}

inline mbase::string mcp_generate_notification(
    const mbase::string& in_method,
    const mbase::Json& in_params = mbase::Json()
)
{
    // Spec: https://modelcontextprotocol.io/specification/2024-11-05/basic/messages#notifications
    mbase::Json responsePacket;
    responsePacket["jsonrpc"] = "2.0";
    responsePacket["method"] = in_method;
    if(in_params.isObject())
    {
        responsePacket["params"] = in_params;
    }

    return responsePacket.toString() + '\n';
}

inline mbase::string mcp_create_request(
    const mbase::string& in_id,
    const mbase::string& in_method,
    const mbase::Json& in_params = mbase::Json()
)
{
    mbase::Json requestPacket;
    requestPacket["jsonrpc"] = "2.0";
    requestPacket["id"] = in_id;
    requestPacket["method"] = in_method;
    if(in_params.isObject())
    {
        requestPacket["params"] = in_params;
    }

    return requestPacket.toString() + '\n';
}

inline mbase::mcp_err_format mcp_extract_error_information(
    mbase::Json& in_error
)
{
    if(!in_error["error"].isObject())
    {
        return mbase::mcp_err_format();
    }

    mbase::mcp_err_format errorFormatOut;

    if(in_error["error"]["code"].isLong())
    {
        errorFormatOut.mCode = in_error["error"]["code"].getLong();
    }

    if(in_error["error"]["message"].isString())
    {
        errorFormatOut.mMessage = in_error["error"]["message"].getString();
    }

    errorFormatOut.mData = in_error["error"]["data"];
    return errorFormatOut;
}

MBASE_END

#endif // MBASE_MCP_PACKET_PARSING_H