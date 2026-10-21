#include "TextPreprocessor.h"
#include <cctype>
#include <vector>
namespace mfe
{
    std::string normalizeText(const std::string &s)
    {
        std::string o;
        bool space = true;
        const std::size_t n = s.size();
        for (std::size_t i = 0; i < n;)
        {
            const unsigned char c = static_cast<unsigned char>(s[i]);
            if (c < 0x80)
            {
                if (std::isalnum(c))
                {
                    o += static_cast<char>(std::tolower(c));
                    space = false;
                }
                else if (!space)
                {
                    o += ' ';
                    space = true;
                }
                ++i;
                continue;
            }
            std::size_t sepLen = 0;
            const unsigned char c1 = i + 1 < n ? static_cast<unsigned char>(s[i + 1]) : 0;
            const unsigned char c2 = i + 2 < n ? static_cast<unsigned char>(s[i + 2]) : 0;
            if (c == 0xC2 && c1 >= 0xA0 && c1 <= 0xBF)
                sepLen = 2; // NBSP, « », ¡ ¿ ...
            else if (c == 0xE2 && (c1 == 0x80 || c1 == 0x81))
                sepLen = 3; // U+2000-206F general punctuation
            else if (c == 0xE3 && c1 == 0x80)
                sepLen = 3; // U+3000-303F CJK punctuation
            else if (c == 0xE0 && c1 == 0xA5 && (c2 == 0xA4 || c2 == 0xA5))
                sepLen = 3; // Devanagari danda
            if (sepLen)
            {
                if (!space)
                {
                    o += ' ';
                    space = true;
                }
                i += sepLen;
            }
            else
            {
                o += static_cast<char>(c);
                space = false;
                ++i;
            }
        }
        if (!o.empty() && o.back() == ' ')
            o.pop_back();
        return o;
    }
    static bool digitsOnly(const std::string &l)
    {
        if (l.empty())
            return false;
        for (unsigned char c : l)
        {
            if (!std::isdigit(c))
                return false;
        }
        return true;
    }
    static std::string trim(const std::string &s)
    {
        std::size_t a = 0, b = s.size();
        while (a < b && std::isspace(static_cast<unsigned char>(s[a])))
            ++a;
        while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1])))
            --b;
        return s.substr(a, b - a);
    }
    static void replaceAll(std::string &s, const std::string &from, const std::string &to)
    {
        for (std::size_t p = 0; (p = s.find(from, p)) != std::string::npos; p += to.size())
            s.replace(p, from.size(), to);
    }
    std::string stripSubtitleMarkup(const std::string &input)
    {
        std::string text = input;
        if (text.compare(0, 3, "\xEF\xBB\xBF") == 0)
            text.erase(0, 3);
        std::vector<std::string> lines;
        std::string cur;
        for (char c : text)
        {
            if (c == '\n')
            {
                lines.push_back(trim(cur));
                cur.clear();
            }
            else
                cur += c;
        }
        lines.push_back(trim(cur));
        std::string out;
        bool skipBlock = false;
        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            const std::string &l = lines[i];
            if (l.empty())
            {
                skipBlock = false;
                continue;
            }
            if (skipBlock)
                continue;
            if (l.compare(0, 6, "WEBVTT") == 0 || l.compare(0, 4, "NOTE") == 0 || l.compare(0, 5, "STYLE") == 0 || l.compare(0, 6, "REGION") == 0)
            {
                skipBlock = true;
                continue;
            }
            if (l.find("-->") != std::string::npos)
                continue;
            if (digitsOnly(l))
            {
                std::size_t j = i + 1;
                while (j < lines.size() && lines[j].empty())
                    ++j;
                if (j < lines.size() && lines[j].find("-->") != std::string::npos)
                    continue; // cue number
            }
            std::string t;
            bool inTag = false, inBrace = false;
            for (char c : l)
            {
                if (c == '<')
                    inTag = true;
                else if (c == '>' && inTag)
                    inTag = false;
                else if (c == '{')
                    inBrace = true;
                else if (c == '}' && inBrace)
                    inBrace = false;
                else if (!inTag && !inBrace)
                    t += c;
            }
            replaceAll(t, "&amp;", "&");
            replaceAll(t, "&lt;", "<");
            replaceAll(t, "&gt;", ">");
            replaceAll(t, "&nbsp;", " ");
            t = trim(t);
            if (!t.empty())
            {
                if (!out.empty())
                    out += ' ';
                out += t;
            }
        }
        return out;
    }
}
namespace mfe
{
    std::size_t wordCount(const std::string &input)
    {
        const std::string t = normalizeText(input);
        if (t.empty())
            return 0;
        std::size_t n = 1;
        for (char c : t)
        {
            if (c == ' ')
                ++n;
        }
        return n;
    }
}
