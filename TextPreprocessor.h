#pragma once
#include <string>
namespace mfe
{
    /* Lower-cases ASCII, turns punctuation/whitespace runs (including common
     Unicode punctuation such as curly quotes) into single spaces, trims ends.
     Non-ASCII letters (e.g. Devanagari) are kept byte-for-byte as word
    characters so non-English filter terms can match.*/
    std::string normalizeText(const std::string &input);
    // Removes SRT/VTT structure (BOM, WEBVTT header, cue numbers, timestamps,
    // NOTE/STYLE blocks, <tags>) and returns plain caption text.
    // Number of words in normalizeText(input).
    std::size_t wordCount(const std::string &input);
    std::string stripSubtitleMarkup(const std::string &input);
}
