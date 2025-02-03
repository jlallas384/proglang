#pragma once
#include "SourceFile.h"
#include "SourceLoc.h"

class ErrorReporter {
public:
    void error(SourceFile& Source, const SourceRange& Loc, const std::string& Message);
};