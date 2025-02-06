#pragma once
#include <cstddef>

struct SourceLoc {
    size_t LineNum = -1, Column = -1;
};

struct SourceRange {
    SourceLoc Start, End;
    SourceRange() = default;
    SourceRange(SourceLoc Start, size_t Length) : Start(Start), End{ Start.LineNum, Start.Column + Length } {}
    SourceRange(SourceLoc Start, SourceLoc End) : Start(Start), End(End) {}
};