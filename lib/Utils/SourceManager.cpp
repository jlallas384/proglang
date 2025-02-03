#include "SourceManager.h"
#include <fstream>
#include <filesystem>

const SourceFile& SourceManager::getSourceFromPath(const std::string& Path) {
    const auto AbsolutePath = std::filesystem::absolute(Path).string();

    if (!Sources.contains(AbsolutePath)) {
        std::ifstream File(AbsolutePath);
        const auto SourceCode = std::string(std::istreambuf_iterator(File), std::istreambuf_iterator<char>());
        Sources.emplace(AbsolutePath, SourceFile{SourceCode, AbsolutePath});
    }
    return Sources.at(AbsolutePath);
}
