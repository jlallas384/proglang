#pragma once

#include "AST/Decl.h"
#include <variant>


using FunctionParam = const FunctionDecl::Param*;

using Symbol = std::variant<const LetStmt*, const FunctionDecl*, FunctionParam>;