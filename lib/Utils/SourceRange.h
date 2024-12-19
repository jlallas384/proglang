#pragma once
#include "SourceLoc.h"

struct SourceRange {
    SourceLoc Start, End;
    SourceRange(SourceLoc Start, size_t Length) : Start(Start), End{ Start.LineNum, Start.Column + Length } {}
};