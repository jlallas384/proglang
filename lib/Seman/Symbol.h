#pragma once

#include "AST/Decl.h"
#include <variant>

using FunctionOverloads = std::vector<const FunctionDecl*>;
using FunctionParam = const FunctionDecl::Param*;

using Symbol = std::variant<const LetStmt*, FunctionOverloads, FunctionParam>;