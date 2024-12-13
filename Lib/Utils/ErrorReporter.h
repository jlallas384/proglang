#pragma once
#include "SourceFile.h"
#include "SourceLoc.h"

class ErrorReporter {
public:
    void error(SourceFile& Source, const SourceLoc& Loc, const std::string& Message);
};