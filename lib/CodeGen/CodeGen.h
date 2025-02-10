#pragma once
#include "TypeEmitter.h"
#include "AST/ASTVisitor.h"
#include "Utils/VisitorBase.h"
#include <llvm/IR/IRBuilder.h>
#include <memory>
#include <map>
#include <string_view>

namespace llvm {
    class Value;
    class LLVMContext;
    class Module;
    class Function;
    class BasicBlock;
};

class Seman;
struct BuilderTy;

class CodeGen : public VisitorBase<CodeGen, const AstBase, llvm::Value*>, public AstConstVisitor {
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
    void visit(const IfStmt&) override;
    void visit(const WhileStmt&) override;
    void visit(const FunctionCallExpr&) override;
    void visit(const UnaryOpExpr&) override;
    void visit(const AssignStmt&) override;

private:
    llvm::Function* emitFunctionProto(const FunctionDecl& FunctionDecl);
    llvm::BasicBlock* emitBlock(std::string_view Name) const;
    llvm::Value* emitLogicalAnd(const Expression& Left, const Expression& Right);
    llvm::Value* emitLogicalOr(const Expression& Left, const Expression& Right);
    llvm::Value* emitLValue(const Expression& Expr);
    llvm::Value* emitRValue(const Expression& Expr);
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::unique_ptr<llvm::LLVMContext> Context;
    std::unique_ptr<llvm::Module> TheModule;
    const FunctionDecl* CurrentFunction = nullptr;
    llvm::Function* llvmFunction = nullptr;
    std::unique_ptr<TypeEmitter> TyEmitter;
    std::map<const Nameable*, llvm::Value*> NameValues;
    Seman& SemanInfo;

};