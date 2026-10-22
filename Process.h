#pragma once
#include <string>
#include <vector>
namespace mfe
{
    struct ProcessResult
    {
        int exitCode = -1;
        std::string output, error;
        bool launched = false;
    };
    ProcessResult runProcess(const std::vector<std::string> &args);
}
