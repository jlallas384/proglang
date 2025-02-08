#pragma once
#include "TypeEmitter.h"
#include "AST/ASTVisitor.h"
#include "Utils/VisitorBase.h"
#include <memory>
#include <map>

namespace llvm {
    class Value;
    class LLVMContext;
    class Module;
    class Function;
};

class Seman;

class CodeGen : public VisitorBase<CodeGen, const AstBase, llvm::Value*>, public AstVisitor {
public:
    CodeGen(Seman& SemanInfo) : SemanInfo(SemanInfo) {}
    ~CodeGen() override;
    void doIt(const Module& Module);

    void visit(const FunctionDecl&) override;
    void visit(const LetStmt&) override;
    void visit(const LiteralExpr&) override;
    void visit(const NamedExpr&) override;
    void visit(const BinaryOpExpr&) override;
    void visit(const ReturnStmt&) override;

private:
    llvm::Value* emitLogicalAnd(const Expression& Left, const Expression& Right);
    llvm::Value* emitLogicalOr(const Expression& Left, const Expression& Right);
    llvm::Value* emitLValue(const Expression& Expr);
    llvm::Value* emitRValue(const Expression& Expr);

    std::unique_ptr<llvm::LLVMContext> Context;
    std::unique_ptr<llvm::Module> TheModule;
    const FunctionDecl* CurrentFunction = nullptr;
    llvm::Function* llvmFunction = nullptr;
    std::unique_ptr<TypeEmitter> TyEmitter;
    std::map<const Nameable*, llvm::Value*> NameValues;
    Seman& SemanInfo;

};