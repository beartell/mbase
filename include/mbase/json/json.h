//
// Copyright 2024 Mozilla Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// Modified by Mustafa Emre Erdog on 2024-12-03
// Modifications include: 
// - Substituted data types with MBASE compatible data types
// - Namespace changed from 'jt' to 'mbase' for general integrity
// - Quoted("") current directory includes are replaced by angled(<>) search includes
// - std::vector and std::string are replaced by mbase::vector and mbase::string


#pragma once
#include <map>
#include <mbase/common.h>
#include <mbase/string.h>
#include <mbase/vector.h>

MBASE_BEGIN

class MBASE_API Json
{
  public:
    enum Type
    {
        Null,
        Bool,
        Long,
        Float,
        Double,
        String,
        Array,
        Object
    };

    enum Status
    {
        success,
        bad_double,
        absent_value,
        bad_negative,
        bad_exponent,
        missing_comma,
        missing_colon,
        malformed_utf8,
        depth_exceeded,
        stack_overflow,
        unexpected_eof,
        overlong_ascii,
        unexpected_comma,
        unexpected_colon,
        unexpected_octal,
        trailing_content,
        illegal_character,
        invalid_hex_escape,
        overlong_utf8_0x7ff,
        overlong_utf8_0xffff,
        object_missing_value,
        illegal_utf8_character,
        invalid_unicode_escape,
        utf16_surrogate_in_utf8,
        unexpected_end_of_array,
        hex_escape_not_printable,
        invalid_escape_character,
        utf8_exceeds_utf16_range,
        unexpected_end_of_string,
        unexpected_end_of_object,
        object_key_must_be_string,
        c1_control_code_in_string,
        non_del_c0_control_code_in_string,
    };

  private:
    Type type_;
    union
    {
        bool bool_value;
        F32 float_value;
        F64 double_value;
        I64 long_value;
        mbase::string string_value;
        mbase::vector<Json> array_value;
        std::map<mbase::string, Json> object_value;
    };

  public:
    static MSTRING StatusToString(Status);
    static std::pair<Status, Json> parse(const mbase::string&);

    Json(const Json&);
    Json(Json&&) noexcept;
    Json(U32);
    Json(U64);
    Json(MSTRING);
    Json(const mbase::string&);
    ~Json();

    Json(const std::nullptr_t = nullptr) : type_(Null)
    {
    }

    Json(bool value) : type_(Bool), bool_value(value)
    {
    }

    Json(int value) : type_(Long), long_value(value)
    {
    }

    Json(F32 value) : type_(Float), float_value(value)
    {
    }

    Json(I64 value) : type_(Long), long_value(value)
    {
    }

    Json(F64 value) : type_(Double), double_value(value)
    {
    }

    Json(mbase::string&& value) : type_(String), string_value(std::move(value))
    {
    }

    Type getType() const
    {
        return type_;
    }

    bool isNull() const
    {
        return type_ == Null;
    }

    bool isBool() const
    {
        return type_ == Bool;
    }

    bool isNumber() const
    {
        return isFloat() || isDouble() || isLong();
    }

    bool isLong() const
    {
        return type_ == Long;
    }

    bool isFloat() const
    {
        return type_ == Float;
    }

    bool isDouble() const
    {
        return type_ == Double;
    }

    bool isString() const
    {
        return type_ == String;
    }

    bool isArray() const
    {
        return type_ == Array;
    }

    bool isObject() const
    {
        return type_ == Object;
    }

    bool getBool() const;
    F32 getFloat() const;
    F64 getDouble() const;
    F64 getNumber() const;
    long long getLong() const;
    const mbase::string& getString() const;
    const mbase::vector<Json>& getArray() const;
    const std::map<mbase::string, Json>& getObject() const;
    mbase::string& getString();
    mbase::vector<Json>& getArray();
    std::map<mbase::string, Json>& getObject();

    bool contains(const mbase::string&) const;

    GENERIC setArray();
    GENERIC setObject();

    mbase::string toString() const;
    mbase::string toStringPretty() const;

    Json& operator=(const Json&);
    Json& operator=(Json&&) noexcept;

    Json& operator[](size_t);
    Json& operator[](const mbase::string&);

    operator mbase::string() const
    {
        return toString();
    }

  private:
    GENERIC clear();
    GENERIC marshal(mbase::string&, bool, int) const;
    static GENERIC stringify(mbase::string&, const mbase::string&);
    static GENERIC serialize(mbase::string&, const mbase::string&);
    static Status parse(Json&, MSTRING&, MSTRING, int, int);
};

MBASE_END
