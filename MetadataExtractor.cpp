#include "MetadataExtractor.h"
#include "Json.h"
#include "PathCompat.h"
#include "Process.h"
#include "TextPreprocessor.h"
#include <algorithm>
#include <cctype>
namespace mfe {
static std::string tagValue(const JsonValue* tags, std::initializer_list<const char*> names) {
    if (!tags || !tags->isObject()) return "";
    for (const char* n : names) {
        // ffprobe tag keys vary in case between containers (TITLE / title).
        for (std::size_t i = 0; i < tags->keys.size(); ++i) {
            std::string k = tags->keys[i];
            std::transform(k.begin(), k.end(), k.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            if (k == n && tags->items[i].isString() && !tags->items[i].text.empty()) return tags->items[i].text;
        }
    }
    return "";
}
static std::vector<std::string> splitTags(const std::string& s) {
    std::vector<std::string> out; std::string cur;
    for (char c : s) {
        if (c == ',' || c == ';') { if (!cur.empty()) out.push_back(cur); cur.clear(); }
        else if (!(cur.empty() && std::isspace(static_cast<unsigned char>(c)))) cur += c;
    }
    if (!cur.empty()) out.push_back(cur);
    for (auto& t : out) while (!t.empty() && std::isspace(static_cast<unsigned char>(t.back()))) t.pop_back();
    return out;
}
bool MetadataExtractor::extract(Video& v, std::string* error) const {
    auto p = runProcess({"ffprobe", "-v", "error", "-show_format", "-show_streams", "-of", "json", v.path});
    if (!p.launched || p.exitCode == 127) { if (error) *error = "ffprobe not found. Install FFmpeg and add it to PATH."; return false; }
    if (p.exitCode != 0) { if (error) *error = "ffprobe failed (invalid or unsupported file): " + p.output.substr(0, p.output.find('\n')); return false; }
    // Warnings may precede the JSON on the merged output stream; isolate the object.
    const std::size_t a = p.output.find('{'), b = p.output.rfind('}');
    JsonValue root; std::string perr;
    if (a == std::string::npos || b == std::string::npos || !parseJson(p.output.substr(a, b - a + 1), root, perr)) {
        if (error) *error = "Could not parse ffprobe output: " + perr;
        return false;
    }
    const JsonValue* streams = root.get("streams");
    const JsonValue* format = root.get("format");
    const JsonValue* videoStream = nullptr;
    if (streams && streams->isArray())
        for (const auto& st : streams->items) {
            const JsonValue* t = st.get("codec_type");
            if (t && t->isString() && t->text == "video") { videoStream = &st; break; }   // not simply "first stream"
        }
    if (!videoStream) { if (error) *error = "File contains no video stream."; return false; }
    double d = 0;
    if (const JsonValue* x = videoStream->get("codec_name")) if (x->isString()) v.codec = x->text;
    if (const JsonValue* x = videoStream->get("width")) if (x->asNumber(d)) v.width = static_cast<int>(d);
    if (const JsonValue* x = videoStream->get("height")) if (x->asNumber(d)) v.height = static_cast<int>(d);
    if (format) if (const JsonValue* x = format->get("duration")) if (x->asNumber(d) && d > 0) v.durationSeconds = d;
    if (v.durationSeconds <= 0) if (const JsonValue* x = videoStream->get("duration")) if (x->asNumber(d) && d > 0) v.durationSeconds = d;
    const JsonValue* ftags = format ? format->get("tags") : nullptr;
    v.title = tagValue(ftags, {"title"});
    if (v.title.empty()) v.title = mfe_path::stem(v.path);           // filename fallback is shown, not invented metadata
    v.description = tagValue(ftags, {"description", "synopsis", "comment"});
    v.category = tagValue(ftags, {"genre"});                         // only if the file actually declares one
    v.tags = splitTags(tagValue(ftags, {"keywords", "tags"}));
    // NOTE: ffprobe's creation_time is when the file was muxed, not when the video was
    // published, so it is deliberately NOT used as a publication date.
    extractCaptions(v);
    return true;
}
bool MetadataExtractor::extractCaptions(Video& v) const {
    auto p = runProcess({"ffmpeg", "-nostdin", "-v", "error", "-i", v.path, "-map", "0:s:0", "-f", "srt", "-"});
    if (p.launched && p.exitCode == 0 && !p.output.empty()) {
        v.captions = stripSubtitleMarkup(p.output);
        return !v.captions.empty();
    }
    return false;
}
}
