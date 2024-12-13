#pragma once
#include <string>

class SourceFile {
public:
    static SourceFile fromPath(std::string SourcePath);
    SourceFile(std::string SourceCode, std::string SourcePath) : SourceCode(std::move(SourceCode)), SourcePath(std::move(SourcePath)) {}
    [[nodiscard]] char at(size_t Index) const {
        return SourceCode[Index];
    }
    [[nodiscard]] size_t length() const {
        return SourceCode.size();
    }
private:
    std::string SourceCode, SourcePath;
};