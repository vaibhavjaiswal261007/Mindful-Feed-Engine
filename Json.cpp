#include "Json.h"
#include <cstdlib>
#include <cmath>
namespace mfe {
const JsonValue* JsonValue::get(const std::string& key) const {
    if (type != Type::Object) return nullptr;
    for (std::size_t i = 0; i < keys.size(); ++i)
        if (keys[i] == key) return &items[i];
    return nullptr;
}
bool JsonValue::asNumber(double& out) const {
    if (type == Type::Number) { out = number; return true; }
    if (type == Type::String && !text.empty()) {
        char* end = nullptr;
        const double d = std::strtod(text.c_str(), &end);
        if (end && *end == '\0' && std::isfinite(d)) { out = d; return true; }
    }
    return false;
}
namespace {
struct Parser {
    const std::string& s; std::size_t i = 0; std::string err;
    explicit Parser(const std::string& in) : s(in) {}
    bool fail(const std::string& m) { if (err.empty()) err = m + " at offset " + std::to_string(i); return false; }
    void ws() { while (i < s.size() && (s[i]==' '||s[i]=='\t'||s[i]=='\n'||s[i]=='\r')) ++i; }
    static void utf8(std::string& o, unsigned cp) {
        if (cp < 0x80) o += static_cast<char>(cp);
        else if (cp < 0x800) { o += static_cast<char>(0xC0|(cp>>6)); o += static_cast<char>(0x80|(cp&0x3F)); }
        else if (cp < 0x10000) { o += static_cast<char>(0xE0|(cp>>12)); o += static_cast<char>(0x80|((cp>>6)&0x3F)); o += static_cast<char>(0x80|(cp&0x3F)); }
        else { o += static_cast<char>(0xF0|(cp>>18)); o += static_cast<char>(0x80|((cp>>12)&0x3F)); o += static_cast<char>(0x80|((cp>>6)&0x3F)); o += static_cast<char>(0x80|(cp&0x3F)); }
    }
    bool hex4(unsigned& v) {
        if (i + 4 > s.size()) return fail("Bad \\u escape");
        v = 0;
        for (int k = 0; k < 4; ++k) {
            char c = s[i++]; v <<= 4;
            if (c>='0'&&c<='9') v |= c-'0'; else if (c>='a'&&c<='f') v |= c-'a'+10;
            else if (c>='A'&&c<='F') v |= c-'A'+10; else return fail("Bad \\u escape");
        }
        return true;
    }
    bool str(std::string& o) {
        if (i >= s.size() || s[i] != '"') return fail("Expected string");
        ++i;
        while (i < s.size()) {
            char c = s[i++];
            if (c == '"') return true;
            if (static_cast<unsigned char>(c) < 0x20) return fail("Control character in string");
            if (c != '\\') { o += c; continue; }
            if (i >= s.size()) break;
            char e = s[i++];
            switch (e) {
                case '"': o += '"'; break; case '\\': o += '\\'; break; case '/': o += '/'; break;
                case 'b': o += '\b'; break; case 'f': o += '\f'; break; case 'n': o += '\n'; break;
                case 'r': o += '\r'; break; case 't': o += '\t'; break;
                case 'u': { unsigned cp; if (!hex4(cp)) return false;
                    if (cp >= 0xD800 && cp < 0xDC00 && i + 1 < s.size() && s[i]=='\\' && s[i+1]=='u') {
                        i += 2; unsigned lo; if (!hex4(lo)) return false;
                        if (lo >= 0xDC00 && lo < 0xE000) cp = 0x10000 + ((cp-0xD800)<<10) + (lo-0xDC00);
                    }
                    utf8(o, cp); break; }
                default: return fail("Bad escape");
            }
        }
        return fail("Unterminated string");
    }
    bool value(JsonValue& v, int depth) {
        if (depth > 64) return fail("Nesting too deep");
        ws();
        if (i >= s.size()) return fail("Unexpected end of input");
        char c = s[i];
        if (c == '{') {
            v.type = JsonValue::Type::Object; ++i; ws();
            if (i < s.size() && s[i] == '}') { ++i; return true; }
            for (;;) {
                ws(); std::string k; if (!str(k)) return false; ws();
                if (i >= s.size() || s[i] != ':') return fail("Expected ':'");
                ++i; JsonValue child; if (!value(child, depth+1)) return false;
                v.keys.push_back(std::move(k)); v.items.push_back(std::move(child)); ws();
                if (i < s.size() && s[i] == ',') { ++i; continue; }
                if (i < s.size() && s[i] == '}') { ++i; return true; }
                return fail("Expected ',' or '}'");
            }
        }
        if (c == '[') {
            v.type = JsonValue::Type::Array; ++i; ws();
            if (i < s.size() && s[i] == ']') { ++i; return true; }
            for (;;) {
                JsonValue child; if (!value(child, depth+1)) return false;
                v.items.push_back(std::move(child)); ws();
                if (i < s.size() && s[i] == ',') { ++i; continue; }
                if (i < s.size() && s[i] == ']') { ++i; return true; }
                return fail("Expected ',' or ']'");
            }
        }
        if (c == '"') { v.type = JsonValue::Type::String; return str(v.text); }
        if (s.compare(i, 4, "true") == 0) { v.type = JsonValue::Type::Bool; v.boolean = true; i += 4; return true; }
        if (s.compare(i, 5, "false") == 0) { v.type = JsonValue::Type::Bool; v.boolean = false; i += 5; return true; }
        if (s.compare(i, 4, "null") == 0) { i += 4; return true; }
        if (c == '-' || (c >= '0' && c <= '9')) {
            char* end = nullptr; v.number = std::strtod(s.c_str() + i, &end);
            if (end == s.c_str() + i) return fail("Bad number");
            v.type = JsonValue::Type::Number; i = static_cast<std::size_t>(end - s.c_str()); return true;
        }
        return fail("Unexpected character");
    }
};
}
bool parseJson(const std::string& input, JsonValue& out, std::string& error) {
    std::size_t start = (input.size() >= 3 && input.compare(0, 3, "\xEF\xBB\xBF") == 0) ? 3 : 0; // skip BOM
    std::string body = input.substr(start);
    Parser p(body); out = JsonValue();
    if (!p.value(out, 0)) { error = p.err; return false; }
    p.ws();
    if (p.i != body.size()) { error = "Trailing characters after JSON value at offset " + std::to_string(p.i); return false; }
    return true;
}
}
