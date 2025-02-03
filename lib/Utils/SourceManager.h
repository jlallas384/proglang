#pragma once

#include "SourceFile.h"
#include <string>
#include <map>

class SourceManager {
public:
    const SourceFile& getSourceFromPath(const std::string &Path);
private:
    std::map<std::string, SourceFile> Sources;
};