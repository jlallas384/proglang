#pragma once
#include <cstddef>

struct SourceLoc {
    size_t LineNum, Column;
};

struct SourceRange {
    SourceLoc Start, End;
    SourceRange(SourceLoc Start, size_t Length) : Start(Start), End{ Start.LineNum, Start.Column + Length } {}
    SourceRange(SourceLoc Start, SourceLoc End) : Start(Start), End(End) {}
};