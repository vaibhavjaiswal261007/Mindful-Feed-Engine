#pragma once
#include "Video.h"
#include <string>
#include <vector>
namespace mfe {
struct ScanReport { std::vector<Video> videos; std::vector<std::string> warnings; };
class VideoLibraryScanner {
public: explicit VideoLibraryScanner(std::string captionDirectory=""); ScanReport scan(const std::string& directory) const;
private: std::string captions_; 
};
}
