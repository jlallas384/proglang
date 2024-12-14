#include "ErrorReporter.h"
#include <iostream>

void ErrorReporter::error(SourceFile& Source, const SourceLoc& Loc, const std::string& Message) {
    std::cout << Loc.LineNum << ":" << Loc.Column << ": " << Message << '\n';
    exit(0);
}
