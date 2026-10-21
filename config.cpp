#pragma once
// Minimal JSON reader (no external dependency). Handles escapes, \uXXXX,
// nested objects/arrays and reports errors. Replaces regex-based scraping,
// which broke on escaped quotes and picked the wrong stream's values.
#include <string>
#include <vector>
namespace mfe
{
    struct JsonValue
    {
        enum class Type
        {
            Null,
            Bool,
            Number,
            String,
            Array,
            Object
        };
        Type type = Type::Null;
        bool boolean = false;
        double number = 0;
        std::string text;
        std::vector<JsonValue> items;  // Array elements, or Object values
        std::vector<std::string> keys; // Object keys (parallel to items)

        bool isObject() const { return type == Type::Object; }
        bool isArray() const { return type == Type::Array; }
        bool isString() const { return type == Type::String; }
        bool isNumber() const { return type == Type::Number; }
        const JsonValue *get(const std::string &key) const; // nullptr if absent
        // Returns text for strings, numeric value for numbers/numeric strings.
        bool asNumber(double &out) const;
    };
    bool parseJson(const std::string &input, JsonValue &out, std::string &error);
}