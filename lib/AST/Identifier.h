#pragma once

#include "Utils/SourceLoc.h"
#include <string>

class IdentifierSymbol {
public:
    IdentifierSymbol(std::string Name, const SourceRange &Range) : Name(std::move(Name)), Range(Range) {}
    const std::string& getName() const { return Name; }
    const SourceRange& getRange() const { return Range; }
private:
    std::string Name;
    SourceRange Range;
};