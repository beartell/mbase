============
JSON Library
============

Welcome to the MBASE JSON library documentation!

-------------------
Document Navigation
-------------------

This documentation is designed to be a single-page document because the library is relatively simple.

-----
About
-----

MBASE JSON library is a lightweight JSON library which internally is the modified version
of the https://github.com/jart/json.cpp.git library.

---------------
Finding the SDK
---------------

If you have installed the MBASE SDK, you can find the library
using CMake :code:`find_package` function with components specification.

In order to find the library using cmake, write the following:

.. code-block:: cmake
    
    find_package(mbase.libs REQUIRED COMPONENTS json)

Specify the include path and link the libraries:

.. code-block:: cmake

    target_include_directories(<your_target> PUBLIC mbase-json)
    target_link_libraries(<your_target> PRIVATE mbase-json)

------------
Header Files
------------

You only need to include single header file called :code:`mbase/json/json.h` which contains a 
single class called :code:`Json`.

-----
Usage
-----

^^^^^^^^^^^^^^
String to JSON
^^^^^^^^^^^^^^

The parser method is a static method :code:`parse` which is defined under the :code:`Json` class
as follows:

.. code-block:: cpp
    :caption: json.h

    class MBASE_API Json
    {
    public:
        ...

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

        ...
    public:
        static MSTRING StatusToString(Status);
        static std::pair<Status, Json> parse(const mbase::string&);

        ...
    };

Which implies a usage such as:

.. code-block:: cpp
    
    #include <mbase/json/json.h>
    #include <iostream>

    int main()
    {
        mbase::string rawJsonString = "{\"first_name\": \"John\", \"last_name\" : \"Doe\", \"id\" : 123}";
        std::pair<mbase::Json::Status, mbase::Json> parseResult = mbase::Json::parse(rawJsonString);

        if(parseResult.first == mbase::Json::Status::success)
        {
            mbase::Json& parsedJson = parseResult.second;
            std::cout << parsedJson["first_name"].getString() << std::endl;
            std::cout << parsedJson["last_name"].getString() << std::endl;
            std::cout << parsedJson["id"].getLong() << std::endl;
        }

        return 0;
    }

^^^^^^^^^^^^^^
JSON to String
^^^^^^^^^^^^^^

Two methods are associated with JSON to string conversion which are defined as:

.. code-block:: cpp
    :caption: json.h

    class MBASE_API Json
    {
    public:
        ...
        mbase::string toString() const;
        mbase::string toStringPretty() const;
        ...
    };

^^^^^^^^^^^^^^^^^^^^^^
Key/Value Manipulation
^^^^^^^^^^^^^^^^^^^^^^

Here are the methods and overloads that are associated with Key/Value manipulation:

.. code-block:: cpp
    :caption: json.h

    class MBASE_API Json
    {
    public:
        ...
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

        GENERIC setArray();
        GENERIC setObject();

        Json& operator=(const Json&);
        Json& operator=(Json&&) noexcept;
        Json& operator[](size_t);
        Json& operator[](const mbase::string&);
        ...
    };

Here is an example usage of primitive value manipulation:

.. code-block:: cpp

    mbase::Json sampleJson;
    sampleJson["null_val"] = nullptr;
    sampleJson["bool_val"] = true; // or false
    sampleJson["long_val"] = 100; // arbitrary number
    sampleJson["float_val"] = 100.0f;
    sampleJson["double_val"] = 100.0f;
    sampleJson["string_val"] = "Sample string";
    sampleJson["empty_array"].setArray();
    sampleJson["empty_object"].setObject();
    

Nested key manip:

.. code-block:: cpp

    sampleJson["nest1"]["nest2"]["nest3"]["long_val"] = 100;

Appending to an array:

.. code-block:: cpp

    mbase::Json sampleJson;
    sampleJson["numbers_array"].setArray();
    sampleJson["strings_array"].setArray();
    mbase::vector<mbase::Json>& numbersArray = sampleJson["numbers_array"].getArray();
    mbase::vector<mbase::Json>& stringsArray = sampleJson["strings_array"].getArray();

    numbersArray.push_back(1);
    numbersArray.push_back(2);
    numbersArray.push_back(3);
    
    stringsArray.push_back("Hello");
    stringsArray.push_back("World!");
    stringsArray.push_back("!");

    std::cout << sampleJson.toStringPretty() << std::endl;

Where the output is:

.. code-block:: bash

    {
        "numbers_array": [1, 2, 3],
        "strings_array": ["Hello", "World!", "!"]
    }

^^^^^^^^^^^^^^
Reading Values
^^^^^^^^^^^^^^

.. important::

    Attempting to read a non-existent value using a :code:`get*` method will crash the application. 
    Make sure to check the value's validity by calling the corresponding :code:`is*` method first.

Here are the methods that are associated with value reading:

.. code-block:: cpp
    :caption: json.h

    class MBASE_API Json
    {
    public:
        ...
        bool isNull() const;
        bool isBool() const;
        bool isNumber() const;
        bool isLong() const;
        bool isFloat() const;
        bool isDouble() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;

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
        ...
    };

And the usage is:

.. code-block:: cpp
    
    mbase::Json sampleJson;
    sampleJson["null_val"] = nullptr;
    sampleJson["bool_val"] = true; // or false
    sampleJson["long_val"] = 100; // arbitrary number
    sampleJson["float_val"] = 100.0f;
    sampleJson["double_val"] = 100.0f;
    sampleJson["string_val"] = "Sample string";
    sampleJson["empty_array"].setArray();
    sampleJson["empty_object"].setObject();

    if(sampleJson["null_val"].isNull())
    {
        std::cout << "null_val is null" << std::endl;
    }

    if(sampleJson["bool_val"].isBool())
    {
        std::cout << "bool_val is " << sampleJson["bool_val"].getBool() << std::endl;
    }

    if(sampleJson["long_val"].isLong())
    {
        std::cout << "long_val is " << sampleJson["long_val"].getLong() << std::endl;
    }

    if(sampleJson["float_val"].isFloat())
    {
        std::cout << "float_val is " << sampleJson["float_val"].getFloat() << std::endl;
    }

    if(sampleJson["double_val"].isDouble())
    {
        std::cout << "double_val is " << sampleJson["double_val"].getDouble() << std::endl;
    }

    if(sampleJson["string_val"].isString())
    {
        std::cout << "string_val is " << sampleJson["string_val"].getString() << std::endl;
    }

    if(sampleJson["empty_array"].isArray())
    {
        std::cout << "empty_array type is array" << std::endl;
        for(auto &currentVal : sampleJson["empty_array"].getArray())
        {
            // ...
        }
    }

    if(sampleJson["empty_object"].isObject())
    {
        std::cout << "empty_object is an object" << std::endl;
        for(auto &objMap : sampleJson["empty_object"].getObject())
        {
            // ...
        }
    }