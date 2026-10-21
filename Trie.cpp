#include "Trie.h"
#include "TextPreprocessor.h"
#include <algorithm>
namespace mfe
{
    Trie::Node *Trie::child(Node &n, char c, bool create)
    {
        for (auto &p : n.children)
            if (p.first == c)
                return p.second.get();
        if (!create)
            return nullptr;
        n.children.push_back({c, std::make_unique<Node>()});
        return n.children.back().second.get();
    }
    const Trie::Node *Trie::child(const Node &n, char c) const
    {
        for (const auto &p : n.children)
            if (p.first == c)
                return p.second.get();
        return nullptr;
    }
    bool Trie::insert(const std::string &raw)
    {
        auto t = normalizeText(raw);
        if (t.empty())
            return false;
        Node *n = &root_;
        for (char c : t)
            n = child(*n, c, true);
        if (n->terminal)
            return false;
        n->terminal = true;
        n->term = t;
        ++count_;
        return true;
    }
    bool Trie::remove(const std::string &raw)
    {
        auto t = normalizeText(raw);
        Node *n = &root_;
        for (char c : t)
        {
            n = child(*n, c, false);
            if (!n)
                return false;
        }
        if (!n->terminal)
            return false;
        n->terminal = false;
        n->term.clear();
        --count_;
        return true;
    }
    std::vector<std::string> Trie::findMatches(const std::string &raw) const
    {
        std::string t = normalizeText(raw);
        std::vector<std::string> out;
        for (size_t i = 0; i < t.size();)
        {
            if (i > 0 && t[i - 1] != ' ')
            {
                ++i;
                continue;
            }
            size_t j = i;
            const Node *n = &root_;
            std::string found;
            size_t end = i;
            while (j < t.size() && (n = child(*n, t[j])))
            {
                ++j;
                if (n->terminal && (i == 0 || t[i - 1] == ' ') && (j == t.size() || t[j] == ' '))
                {
                    found = n->term;
                    end = j;
                }
            }
            if (!found.empty())
            {
                out.push_back(found);
                i = end;
            }
            else
                ++i;
        }
        std::sort(out.begin(), out.end());
        out.erase(std::unique(out.begin(), out.end()), out.end());
        return out;
    }
    std::vector<FilterMatch> Trie::findMatches(const std::string &t, const std::string &f, const std::string &r) const
    {
        std::vector<FilterMatch> o;
        for (const auto &m : findMatches(t))
            o.push_back({m, f, r});
        return o;
    }
    std::vector<TermCount> Trie::countMatches(const std::string &raw) const
    {
        std::string t = normalizeText(raw);
        std::vector<TermCount> out;
        for (size_t i = 0; i < t.size();)
        {
            if (i > 0 && t[i - 1] != ' ')
            {
                ++i;
                continue;
            }
            size_t j = i;
            const Node *n = &root_;
            std::string found;
            size_t end = i;
            while (j < t.size() && (n = child(*n, t[j])))
            {
                ++j;
                if (n->terminal && (j == t.size() || t[j] == ' '))
                {
                    found = n->term;
                    end = j;
                }
            }
            if (!found.empty())
            {
                bool done = false;
                for (auto &c : out)
                    if (c.term == found)
                    {
                        ++c.count;
                        done = true;
                        break;
                    }
                if (!done)
                    out.push_back({found, 1});
                i = end;
            }
            else
                ++i;
        }
        return out;
    }
}