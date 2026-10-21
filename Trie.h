#pragma once
#include "Video.h"
#include <memory>
#include <string>
#include <vector>
namespace mfe
{
    class Trie
    {
        struct Node
        {
            bool terminal = false;
            std::string term;
            std::vector<std::pair<char, std::unique_ptr<Node>>> children;
        };
        Node root_;
        std::size_t count_ = 0;
        Node *child(Node &, char, bool create);
        const Node *child(const Node &, char) const;

    public:
        bool insert(const std::string &term);
        bool remove(const std::string &term);
        std::vector<std::string> findMatches(const std::string &text) const;
        std::vector<FilterMatch> findMatches(const std::string &text, const std::string &field, const std::string &reason) const;
        // Counts EVERY occurrence (non-overlapping, longest phrase wins) per term, unlike findMatches which de-duplicates.
        std::vector<TermCount> countMatches(const std::string &text) const;
        std::size_t size() const { return count_; }
    };
}