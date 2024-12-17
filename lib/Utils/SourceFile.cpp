#include "SourceFile.h"
#include <fstream>

SourceFile SourceFile::fromPath(std::string SourcePath) {
    std::ifstream File(SourcePath);
    const auto SourceCode = std::string(std::istreambuf_iterator(File), std::istreambuf_iterator<char>());
    return { SourceCode, std::move(SourcePath) };
}
