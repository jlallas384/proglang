#include "SourceInfo.h"
#include <fstream>


SourceInfo SourceInfo::fromPath(std::string SourcePath) {
    std::ifstream File(SourcePath);
    const auto SourceCode = std::string(std::istreambuf_iterator(File), std::istreambuf_iterator<char>());
    return { SourceCode, std::move(SourcePath) };
}
