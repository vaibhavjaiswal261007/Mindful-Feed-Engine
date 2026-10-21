#pragma once
#include "ContentAnalyzer.h"
#include "RankingEngine.h"
#include "Trie.h"
#include <string>
namespace mfe {
struct AppConfig {
    std::string libraryDirectory = "videos", captionDirectory = "transcripts", filterTermsFile = "config/filter_terms.txt", brainrotTermsFile = "config/brainrot_terms.txt"; // empty brainrotTermsFile = penalty disabled
    std::size_t sessionTokens = 5;
    bool rejectAnyMatch = true, allowRepeat = false;
    std::string breakMessage = "You've reached your viewing limit. Take a mindful break.";
    std::string logLevel = "warn";   // error | warn | info | debug
    AnalyzerConfig analyzer; RankingConfig ranking;
};
// Parses and validates the JSON config. Unknown keys are ignored; wrong types/ranges are errors.
bool loadConfig(const std::string& path, AppConfig& config, std::string& error);
// Loads one term/phrase per line ('#' comments and blank lines ignored; CRLF/BOM safe).
bool loadFilterTerms(const std::string& path, Trie& trie, std::size_t& added, std::string& error);
}