set(sources
    "AST/Decl.cpp"
    "AST/Decl.h"
    "AST/Expr.cpp"
    "AST/Expr.h"
    "AST/Stmt.cpp"
    "AST/Stmt.h"
    "AST/ASTBase.h"
    "AST/Visitor.h"
    "AST/Module.cpp"
    "AST/Module.h"
    "AST/ASTPrinter.h"
    "AST/ASTPrinter.cpp"

    "Parser/Token.h"
    "Parser/Lexer.cpp"
    "Parser/Lexer.h"
    "Parser/Parser.cpp"
    "Parser/Parser.h"

    "Types/Type.h"
    "Types/TypesManager.cpp"
    "Types/TypesManager.h"
    "Types/FloatingPoint.h"
    "Types/Integer.h"

    "Utils/SourceFile.cpp"
    "Utils/SourceLoc.h"
    "Utils/ErrorReporter.cpp"
    "Utils/ErrorReporter.h"
 "Types/Type.cpp" "Utils/SourceRange.h" "AST/Visitor.cpp" "AST/UnresolvedType.h")

add_library(Lib ${sources})

target_include_directories(Lib PUBLIC .)
