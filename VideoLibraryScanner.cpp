#include "VideoLibraryScanner.h"
#include "MetadataExtractor.h"
#include "PathCompat.h"
#include "TextPreprocessor.h"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <set>

namespace mfe {
VideoLibraryScanner::VideoLibraryScanner(std::string c) : captions_(std::move(c)) {}

static std::string readFile(const std::string& path) {
    std::ifstream f(path.c_str(), std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

// Stable ID from file CONTENT (size + first/last 1 MiB, FNV-1a 64-bit), so it does not
// change when the folder is moved/renamed or files are enumerated in another order,
// and byte-identical copies are detected as duplicates.
static bool fingerprint(const std::string& path, std::string& idOut) {
    std::ifstream f(path.c_str(), std::ios::binary);
    if (!f) return false;
    f.seekg(0, std::ios::end);
    const std::streamoff size = f.tellg();
    if (size < 0) return false;
    std::uint64_t h = 1469598103934665603ULL;
    auto feed = [&h](const char* data, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) { h ^= static_cast<unsigned char>(data[i]); h *= 1099511628211ULL; }
    };
    const std::streamoff chunk = 1 << 20;
    std::string buf(static_cast<std::size_t>(std::min(size, chunk)), '\0');
    std::uint64_t sz = static_cast<std::uint64_t>(size);
    for (int i = 0; i < 8; ++i) { char b = static_cast<char>((sz >> (8 * i)) & 0xFF); feed(&b, 1); }
    f.seekg(0); f.read(&buf[0], static_cast<std::streamsize>(buf.size())); feed(buf.data(), static_cast<std::size_t>(f.gcount()));
    if (size > chunk) {
        f.clear(); f.seekg(size - chunk); f.read(&buf[0], static_cast<std::streamsize>(buf.size()));
        feed(buf.data(), static_cast<std::size_t>(f.gcount()));
    }
    static const char* hex = "0123456789abcdef";
    std::string id = "v";
    for (int shift = 60; shift >= 0; shift -= 4) id += hex[(h >> shift) & 0xF];
    idOut = id;
    return true;
}

ScanReport VideoLibraryScanner::scan(const std::string& dir) const {
    ScanReport r;
    if (!mfe_path::exists(dir) || !mfe_path::isDirectory(dir)) {
        r.warnings.push_back("Video directory does not exist or is not a directory: " + dir);
        return r;
    }
    MetadataExtractor ex;
    std::vector<std::string> paths;
    mfe_path::findMp4Files(dir, paths, r.warnings);
    std::sort(paths.begin(), paths.end());
    std::set<std::string> seen;
    for (const std::string& path : paths) {
        Video v;
        v.path = mfe_path::absolute(path);
        if (!fingerprint(v.path, v.id)) { r.warnings.push_back(mfe_path::filename(path) + ": cannot read file."); continue; }
        if (!seen.insert(v.id).second) {
            r.warnings.push_back("Duplicate content skipped: " + mfe_path::filename(path));
            continue;
        }
        std::string err;
        if (!ex.extract(v, &err)) { r.warnings.push_back(mfe_path::filename(path) + ": " + err); continue; }
        if (!captions_.empty()) {
            const std::string base = mfe_path::join(captions_, mfe_path::stem(path));
            const char* extensions[] = {".srt", ".vtt"};
            for (const char* e : extensions) {
                const std::string captionPath = base + e;
                if (mfe_path::exists(captionPath)) {
                    const std::string clean = stripSubtitleMarkup(readFile(captionPath));
                    if (!clean.empty()) v.captions = clean;   // external file overrides embedded captions
                    break;
                }
            }
        }
        r.videos.push_back(std::move(v));
    }
    return r;
}
}
