#pragma once
#include "SourceLoc.h"

#include <string>
#include <string_view>
#include <vector>
#include <span>

class SourceFile {
public:
    SourceFile(std::string SrcString, std::string SourcePath);

    [[nodiscard]] char at(size_t Index) const {
        return SourceCode[Index];
    }
    [[nodiscard]] size_t length() const {
        return SourceCode.size();
    }
    std::string getSourcePath() const { return SourcePath; }
    std::span<std::string_view> getSourceFromRange(const SourceRange &Range);
private:
    std::string SourceCode, SourcePath;
    std::vector<std::string_view> SourceLines;
};
