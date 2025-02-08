set(sources
    "AST/Decl.cpp"
    "AST/Decl.h"
    "AST/Expr.cpp"
    "AST/Expr.h"
    "AST/Stmt.cpp"
    "AST/Stmt.h"
    "AST/ASTBase.h"
    "AST/ASTVisitor.h"
    "AST/Module.cpp"
    "AST/Module.h"
    "AST/ASTPrinter.h"
    "AST/ASTPrinter.cpp"
    "AST/Type.h"
    "AST/Type.cpp"
    "AST/TypeContext.cpp"
    "AST/ASTVisitor.cpp"
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
    "AST/TypeVisitor.h"
    "Seman/TypeResolver.h"
    "Seman/TypeResolver.cpp"
    "Utils/VisitorBase.h"
    "Seman/NameResolver.h"
    "Seman/NameResolver.cpp"
    "Seman/Scope.h"
    
    
    "Seman/Validator.h"
    "Seman/Validator.cpp"
    "Seman/TypeCheck.h"
    "Seman/TypeCheck.cpp"
    "Utils/SourceManager.h" 
    "Utils/SourceManager.cpp" "AST/Identifier.h"
    "Parser/TokenKind.h"    
 "Seman/TypeValidator.h" "Seman/TypeValidator.cpp" "CodeGen/TypeEmitter.h" "CodeGen/TypeEmitter.cpp" "CodeGen/CodeGen.h" "CodeGen/CodeGen.cpp")

add_library(Lib ${sources})
find_package(LLVM CONFIG REQUIRED)

target_include_directories(Lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${LLVM_INCLUDE_DIRS})
llvm_map_components_to_libnames(llvm_libs Core)
target_link_libraries(Lib PRIVATE ${llvm_libs})