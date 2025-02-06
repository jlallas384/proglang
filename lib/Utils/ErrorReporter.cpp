#include "ErrorReporter.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstdint>
#include <format>

enum class UnderlineState : std::uint8_t {
    Not,
    Middle,
    Done
};


std::vector<std::string> createUnderlines(std::span<std::string_view> Lines, const SourceRange& Loc) {
    unsigned LineIndex = 0;
    auto State = UnderlineState::Not;
    auto NumLines = Lines.size();
    std::vector<std::string> Ret;

    for (auto Line : Lines) {
        std::ostringstream Underline;
        bool IsLastLine = LineIndex + 1 == NumLines;

        for (unsigned Idx = 0; Idx < Line.size(); Idx++) {
            switch (State) {
                case UnderlineState::Not:
                    if (Loc.Start.Column == Idx) {
                        Underline << "^";
                        State = UnderlineState::Middle;
                        if (IsLastLine && Idx == Loc.End.Column) {
                            State = UnderlineState::Done;
                        }
                    }
                    else {
                        if (Line[Idx] == '\t') {
                            Underline << "    ";
                        }
                        else {
                            Underline << " ";
                        }
                    }
                    break;
                case UnderlineState::Middle:
                    if (IsLastLine && Idx == Loc.End.Column) {
                        State = UnderlineState::Done;
                        break;
                    }
                    if (Line[Idx] == '\t') {
                        Underline << "~~~~";
                    }
                    else {
                        Underline << "~";
                    }
                    break;
                case UnderlineState::Done:
                    break;
            }
        }
        Ret.push_back(Underline.str());
        LineIndex++;
    }
    return Ret;
}

std::vector<std::string> createLineNumPrefix(const SourceRange& Loc, std::string& UnderlinePrefix) {
    auto IntLength = [](auto Int) {
        return std::to_string(Int).size();
    };
    auto Width = IntLength(Loc.End.LineNum);
    std::vector<std::string> Ret;

    UnderlinePrefix = std::format("{:>{}} | ", "", Width);
    for (auto LineNum = Loc.Start.LineNum; LineNum <= Loc.End.LineNum; LineNum++) {
        auto Prefix = std::format("{:>{}} | ", LineNum, Width);
        Ret.push_back(Prefix);
    }
    return Ret;
}

void ErrorReporter::error(SourceFile& Source, const SourceRange& Loc, const std::string& Message) {
    auto Msg = std::format("error: {}:{}:{}: {}", Source.getSourcePath(), Loc.Start.LineNum, Loc.Start.Column + 1, Message);
    std::cout << Msg << '\n';
    auto SourceLines = Source.getSourceFromRange(Loc);
    auto Underlines = createUnderlines(SourceLines, Loc);
    std::string UnderlinePrefix;
    auto LineNumPrefix = createLineNumPrefix(Loc, UnderlinePrefix);

    for (unsigned Idx = 0; Idx < SourceLines.size(); Idx++) {
        std::cout << LineNumPrefix[Idx];
        for (auto Chr : SourceLines[Idx]) {
            if (Chr == '\t') std::cout << "    ";
            else std::cout << Chr;
        }
        std::cout << '\n';
        std::cout << UnderlinePrefix;
        std::cout << Underlines[Idx] << '\n';
    }
    std::cout << '\n';
}
