#include "SourceFile.h"
#include <algorithm>

SourceFile::SourceFile(std::string SrcString, std::string SourcePath): SourceCode(std::move(SrcString)), SourcePath(std::move(SourcePath)) {
    auto Left = SourceCode.begin();
    while (Left != SourceCode.end()) {
        auto Right = std::find(Left, SourceCode.end(), '\n');
        SourceLines.emplace_back(Left, Right);
        if (Right != SourceCode.end()) {
            ++Right;
        }
        Left = Right;
    }
    SourceLines.push_back("");
}

std::span<std::string_view> SourceFile::getSourceFromRange(const SourceRange &Range) {
    return { &SourceLines[Range.Start.LineNum - 1], &SourceLines[Range.End.LineNum] };
}
