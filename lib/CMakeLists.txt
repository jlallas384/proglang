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
    "AST/Type.h"
    "AST/Type.cpp"
    "AST/TypeContext.cpp"
    "AST/Visitor.cpp"

    "Parser/Token.h"
    "Parser/Lexer.cpp"
    "Parser/Lexer.h"
    "Parser/Parser.cpp"
    "Parser/Parser.h"

    "Utils/SourceFile.cpp"
    "Utils/SourceLoc.h"
    "Utils/ErrorReporter.cpp"
    "Utils/ErrorReporter.h"
    
    "Utils/Literal.h"

    "Seman/Seman.h"
    "Seman/Seman.cpp"


)

add_library(Lib ${sources})

find_package(LLVM CONFIG REQUIRED)
target_include_directories(Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

llvm_map_components_to_libnames(llvm_libs Core)
target_link_libraries(Lib PRIVATE ${llvm_libs})
