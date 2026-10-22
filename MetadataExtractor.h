#pragma once
#include "Video.h"
#include <string>
namespace mfe {
class MetadataExtractor {
public: bool extract(Video& video, std::string* error=nullptr) const;
private: bool extractCaptions(Video& video) const;
};
}
