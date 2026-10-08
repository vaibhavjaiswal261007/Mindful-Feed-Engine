#ifndef HASH_FILTER_H
#define HASH_FILTER_H

#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

class HashFilter {
private:
    // tag -> score   e.g. "coding" -> 15, "brainrot" -> -25
    unordered_map<string, int> tagScores;

    // lowercase + trim + strip leading '#', so "#Coding " == "coding"
    static string normalizeTag(const string& tag);

public:
    // Load tag scores from a text file, one "tag,score" per line (e.g. tag_scores.txt)
    void loadTagScores(const string& filename);

    void displayHashMap();
};

#endif