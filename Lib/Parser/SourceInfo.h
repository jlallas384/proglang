#pragma once
#include <string>

class SourceInfo {
public:
    static SourceInfo fromPath(std::string SourcePath);
    SourceInfo(std::string SourceCode, std::string SourcePath) : SourceCode(std::move(SourceCode)), SourcePath(std::move(SourcePath)) {}
    [[nodiscard]] char at(size_t Index) const {
        return SourceCode[Index];
    }
    [[nodiscard]] size_t length() const {
        return SourceCode.size();
    }
private:
    std::string SourceCode, SourcePath;
};