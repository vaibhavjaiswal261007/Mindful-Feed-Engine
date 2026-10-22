#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace mfe {
struct ScoreComponent { std::string name; double raw = 0; double weight = 0; double contribution = 0; std::string explanation; bool applicable = true; };
struct FilterMatch { std::string term, field, reason; };
struct TermCount { std::string term; int count = 0; };
struct Video {
 std::string id, path, title, description, codec, category, publicationDate;
 double durationSeconds = 0; int width = 0, height = 0;
 std::vector<std::string> tags;
 std::string captions;
 std::vector<TermCount> brainrotHits; int captionWords = 0; double brainrotDensity = 0; // hits per 100 caption words (set by scoring)
 bool filtered = false; std::vector<FilterMatch> filterMatches;
 double nutritionalScore = 0, rankingScore = 0;
 std::vector<ScoreComponent> scoreComponents, rankingComponents;
};
}
