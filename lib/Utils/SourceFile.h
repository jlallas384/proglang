#pragma once
#include "SourceLoc.h"

#include <string>
#include <string_view>
#include <vector>
#include <span>

class SourceFile {
public:
    SourceFile(std::string SrcString, std::string SourcePath);

    std::string_view getSourceCode() const { return SourceCode; }
    std::string getSourcePath() const { return SourcePath; }
    std::span<std::string_view> getSourceFromRange(const SourceRange &Range);
private:
    std::string SourceCode, SourcePath;
    std::vector<std::string_view> SourceLines;
};
