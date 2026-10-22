#pragma once

// Small path and directory helpers for compilers without C++17 <filesystem>.
// Uses APIs available in older MinGW/GCC and common POSIX toolchains.
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <sys/stat.h>
#include <dirent.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#include <stdlib.h>
#else
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#endif

namespace mfe_path {
inline std::string join(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (a[a.size()-1] == '/' || a[a.size()-1] == '\\') return a + b;
    return a + "/" + b;
}
inline std::string filename(const std::string& path) {
    const std::string::size_type p = path.find_last_of("/\\");
    return p == std::string::npos ? path : path.substr(p + 1);
}
inline std::string extension(const std::string& path) {
    const std::string name = filename(path);
    const std::string::size_type p = name.find_last_of('.');
    if (p == std::string::npos) return std::string();
    std::string ext = name.substr(p);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext;
}
inline std::string stem(const std::string& path) {
    const std::string name = filename(path);
    const std::string::size_type p = name.find_last_of('.');
    return p == std::string::npos ? name : name.substr(0, p);
}
inline bool isDirectory(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR) != 0;
}
inline bool isRegularFile(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0 && (st.st_mode & S_IFREG) != 0;
}
inline bool exists(const std::string& path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}
inline std::string absolute(const std::string& path) {
#ifdef _WIN32
    char buffer[4096];
    // GetFullPathNameA is available in older MinGW versions where _fullpath
    // may not be declared by the runtime headers.
    const DWORD length = GetFullPathNameA(path.c_str(),
                                          static_cast<DWORD>(sizeof(buffer)),
                                          buffer, NULL);
    if (length > 0 && length < sizeof(buffer)) return std::string(buffer);
#else
    char buffer[PATH_MAX];
    if (realpath(path.c_str(), buffer) != NULL) return std::string(buffer);
#endif
    return path;
}
inline void findMp4Files(const std::string& directory,
                         std::vector<std::string>& files,
                         std::vector<std::string>& warnings,
                         int depth = 0) {
    if (depth > 16) {
        warnings.push_back("Directory nesting too deep (possible symlink loop), skipped: " + directory);
        return;
    }
    DIR* dir = opendir(directory.c_str());
    if (!dir) {
        warnings.push_back("Cannot read directory: " + directory);
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        const std::string name(entry->d_name);
        if (name == "." || name == "..") continue;
        const std::string path = join(directory, name);
        if (isDirectory(path)) {
            findMp4Files(path, files, warnings, depth + 1);
        } else if (isRegularFile(path) && extension(path) == ".mp4") {
            files.push_back(path);
        }
    }
    closedir(dir);
}
}
