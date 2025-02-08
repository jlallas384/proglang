#include "CodeGen.h"
#include "Seman/Seman.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/Verifier.h>
#include <ranges>
#include <iostream>
CodeGen::~CodeGen() = default;

void CodeGen::doIt(const Module& Module) {
    Context = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("", *Context);
    TyEmitter = std::make_unique<TypeEmitter>(*Context, SemanInfo);
    AstVisitor::visit(Module);
    TheModule->dump();
    llvm::verifyModule(*TheModule, &llvm::outs());
    //TheModule->dum
}

void CodeGen::visit(const FunctionDecl& FunctionDecl) {
    const auto FuncTy = llvm::cast<llvm::FunctionType>(TyEmitter->emit(*SemanInfo.getType(FunctionDecl)));
    const auto Func = llvm::Function::Create(FuncTy, llvm::GlobalValue::ExternalLinkage, FunctionDecl.getName(), TheModule.get());
    NameValues[&FunctionDecl] = Func;

    const auto Block = llvm::BasicBlock::Create(*Context, "entry", Func);
    llvm::IRBuilder Builder(&Func->back());
    auto ParamIter = FunctionDecl.getParams().begin();

    for (auto& Arg : Func->args()) {
        const auto ArgTy = Arg.getType();
        ArgTy->dump();
        auto Size = TheModule->getDataLayout().getTypeStoreSize(ArgTy);
        const auto Alloca = Builder.CreateAlloca(ArgTy);
        //Alloca->getType()->dump();
        Builder.CreateStore(&Arg, Alloca);
        NameValues[&*ParamIter] = Alloca;
        ++ParamIter;
    }

    llvmFunction = Func;
    CurrentFunction = &FunctionDecl;
    AstVisitor::visit(FunctionDecl);
    Builder.SetInsertPoint(&Func->back());
}

void CodeGen::visit(const LetStmt& LetStmt) {
    const auto LetTy = SemanInfo.getType(LetStmt);
    auto Func = llvm::cast<llvm::Function>(NameValues[CurrentFunction]);

    llvm::IRBuilder Builder(&Func->back());
    
    const auto Alloca = Builder.CreateAlloca(TyEmitter->emit(*LetTy));
    Alloca->dump();
    Alloca->getType()->dump();
    const auto Value = LetStmt.getValue();
    if (Value) {
        const auto Init = doVisit(*Value);
        Builder.SetInsertPoint(&llvmFunction->back());
        Builder.CreateStore(Init, Alloca);
    }

    NameValues[&LetStmt] = Alloca;
}

void CodeGen::visit(const LiteralExpr& LiteralExpr) {
    if (LiteralExpr.is<IntLiteral>()) {
        const auto I32Ty = llvm::IntegerType::getInt32Ty(*Context);
        return returnValue(llvm::ConstantInt::get(I32Ty, LiteralExpr.as<IntLiteral>().Value, true));
    }
    if (LiteralExpr.is<BoolLiteral>()) {
        return returnValue(llvm::ConstantInt::getBool(llvm::IntegerType::get(*Context, 1), LiteralExpr.as<BoolLiteral>().Value));
    }
}



void CodeGen::visit(const NamedExpr& NamedExpr) {
    const auto RefName = SemanInfo.getReferencedName(NamedExpr.getIdentifier());
    const auto Value = llvm::cast<llvm::AllocaInst>(NameValues[RefName]);

    auto Func = llvm::cast<llvm::Function>(NameValues[CurrentFunction]);
    llvm::IRBuilder Builder(&Func->back());

    for (auto &Param : CurrentFunction->getParams()) {
        if (RefName == &Param) {
            return returnValue(Value);
        }
    }
}



void CodeGen::visit(const BinaryOpExpr& BinaryOpExpr) {
    switch (BinaryOpExpr.getKind()) {
        case TokenKind::AmpAmp:
            return returnValue({ emitLogicalAnd(BinaryOpExpr.getLeft(), BinaryOpExpr.getRight()) });
        case TokenKind::PipePipe:
            return returnValue({ emitLogicalOr(BinaryOpExpr.getLeft(), BinaryOpExpr.getRight()) });
    }
}

void CodeGen::visit(const ReturnStmt& ReturnStmt) {

    auto Ret = emitLValue(*ReturnStmt.getValue());
    llvm::IRBuilder Builder(&llvmFunction->back());
    Builder.CreateRet(Ret);
}


llvm::Value* CodeGen::emitLogicalAnd(const Expression& Left, const Expression& Right) {
    const auto CurBlock = &llvmFunction->back();
    auto LhsBlock = llvm::BasicBlock::Create(*Context, "land.lhs", llvmFunction);
    llvm::IRBuilder Builder(CurBlock);
    Builder.CreateBr(LhsBlock);

    const auto Lft = emitLValue(Left);
    LhsBlock = &llvmFunction->back();
    Builder.SetInsertPoint(LhsBlock);

    auto RhsBlock = llvm::BasicBlock::Create(*Context, "land.rhs", llvmFunction);
    const auto Rgt = emitLValue(Right);

    auto NewRhsBlock = &llvmFunction->back();

    auto Finish = llvm::BasicBlock::Create(*Context, "land.finish", llvmFunction);
    Builder.CreateCondBr(Lft, RhsBlock, Finish);
    Builder.SetInsertPoint(NewRhsBlock);
    Builder.CreateBr(Finish);

    Builder.SetInsertPoint(Finish);
    auto Phi = Builder.CreatePHI(Builder.getInt1Ty(), 2);
    Phi->addIncoming(Builder.getFalse(), LhsBlock);
    Phi->addIncoming(Rgt, NewRhsBlock);
    return Phi;
}

llvm::Value* CodeGen::emitLogicalOr(const Expression& Left, const Expression& Right) {
    const auto CurBlock = &llvmFunction->back();
    auto LhsBlock = llvm::BasicBlock::Create(*Context, "lor.lhs", llvmFunction);
    llvm::IRBuilder Builder(CurBlock);
    Builder.CreateBr(LhsBlock);

    const auto Lft = emitLValue(Left);
    LhsBlock = &llvmFunction->back();
    Builder.SetInsertPoint(LhsBlock);

    auto RhsBlock = llvm::BasicBlock::Create(*Context, "lor.rhs", llvmFunction);
    const auto Rgt = emitLValue(Right);

    auto NewRhsBlock = &llvmFunction->back();

    auto Finish = llvm::BasicBlock::Create(*Context, "lor.finish", llvmFunction);
    Builder.CreateCondBr(Lft, Finish, RhsBlock);
    Builder.SetInsertPoint(NewRhsBlock);
    Builder.CreateBr(Finish);

    Builder.SetInsertPoint(Finish);
    auto Phi = Builder.CreatePHI(Builder.getInt1Ty(), 2);
    Phi->addIncoming(Builder.getTrue(), LhsBlock);
    Phi->addIncoming(Rgt, NewRhsBlock);
    return Phi;
}

llvm::Value* CodeGen::emitLValue(const Expression& Expr) {
    const auto Value = doVisit(Expr);
    if (auto Alloca = llvm::dyn_cast<llvm::AllocaInst>(Value)) {
        llvm::IRBuilder Builder(&llvmFunction->back());
        return Builder.CreateLoad(Alloca->getAllocatedType(), Alloca);
    }
    return Value;
}

llvm::Value* CodeGen::emitRValue(const Expression& Expr) {
    return nullptr;
}












