set(sources 
	"AST/Decl.cpp"
	"AST/Decl.h"
	"AST/Expr.cpp"
	"AST/Expr.h"
	"AST/Stmt.cpp"
	"AST/Stmt.h"
	"Parser/Token.h"
	"Types/Type.h"
	"Types/TypesManager.cpp"
	"Types/TypesManager.h"
	"AST/ASTBase.h"
	"AST/Visitor.h" 
	"Parser/Lexer.cpp" 
	"Parser/Lexer.h"
	"Parser/Parser.cpp" 
	"Parser/Parser.h"
	 
	"Utils/SourceFile.cpp" 
	"AST/Module.cpp" "AST/Module.h" "Types/FloatingPoint.h" "Types/Integer.h"  "Utils/SourceLoc.h" "Utils/ErrorReporter.h" "Utils/ErrorReporter.cpp" "AST/ASTPrinter.h" "AST/ASTPrinter.cpp")

add_library(Lib ${sources})

target_include_directories(Lib PUBLIC .)