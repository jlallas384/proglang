#include "CodeGen.h"
#include "Seman/Seman.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/Verifier.h>

CodeGen::~CodeGen() = default;

void CodeGen::doIt(const Module& Module) {
    Context = std::make_unique<llvm::LLVMContext>();
    TheModule = std::make_unique<llvm::Module>("", *Context);
    TyEmitter = std::make_unique<TypeEmitter>(*Context, SemanInfo);
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
    AstConstVisitor::visit(Module);
    TheModule->dump();
    llvm::verifyModule(*TheModule, &llvm::outs());
    //TheModule->dum
}

void CodeGen::visit(const FunctionDecl& FunctionDecl) {
    const auto Func = emitFunctionProto(FunctionDecl);
    const auto Block = llvm::BasicBlock::Create(*Context, "entry", Func);
    Builder->SetInsertPoint(Block);
    for (const auto [Arg, Param] : zip_equal(Func->args(), FunctionDecl.getParams())) {
        Arg.setName(Param.getName());
        const auto ArgTy = Arg.getType();
        const auto Alloca = Builder->CreateAlloca(ArgTy);
        Alloca->setName(Param.getName() + ".addr");
        Builder->CreateStore(&Arg, Alloca);
        NameValues[&Param] = Alloca;
    }

    llvmFunction = Func;
    CurrentFunction = &FunctionDecl;
    AstConstVisitor::visit(FunctionDecl);

    const auto& LastBlock = llvmFunction->back();
    if (LastBlock.empty() || !LastBlock.back().isTerminator()) {
        Builder->CreateUnreachable();
    }
}

void CodeGen::visit(const LetStmt& LetStmt) {
    const auto LetTy = SemanInfo.getType(LetStmt);
    const auto Ty = TyEmitter->emit(*LetTy);
    const auto Alloca = Builder->CreateAlloca(Ty);
    Alloca->getType()->dump();

    const auto X = llvm::cast<llvm::GetElementPtrInst>(Builder->CreateGEP(Ty, Alloca, { Builder->getInt32(0) }));
    X->getSourceElementType()->dump();
    X->getResultElementType()->dump();

    Alloca->setName(LetStmt.getName());
    if (const auto Value = LetStmt.getValue()) {
        const auto Init = emitRValue(*Value);
        Builder->CreateStore(Init, Alloca);
    }
    NameValues[&LetStmt] = Alloca;
}

void CodeGen::visit(const LiteralExpr& LiteralExpr) {
    if (LiteralExpr.is<IntLiteral>()) {
        return returnValue(Builder->getInt32(LiteralExpr.as<IntLiteral>().Value));
    }
    if (LiteralExpr.is<BoolLiteral>()) {
        return returnValue(Builder->getInt1(LiteralExpr.as<BoolLiteral>().Value));
    }
}

void CodeGen::visit(const NamedExpr& NamedExpr) {
    const auto RefName = SemanInfo.getReferencedName(NamedExpr.getIdentifier());
    if (!NameValues.contains(RefName)) {
        NameValues[RefName] = emitFunctionProto(*static_cast<const FunctionDecl*>(RefName));
    }
    return returnValue(NameValues[RefName]);
}

void CodeGen::visit(const BinaryOpExpr& BinaryOpExpr) {
    switch (BinaryOpExpr.getKind()) {
        case TokenKind::AmpAmp:
            return returnValue(emitLogicalAnd(BinaryOpExpr.getLeft(), BinaryOpExpr.getRight()));
        case TokenKind::PipePipe:
            return returnValue(emitLogicalOr(BinaryOpExpr.getLeft(), BinaryOpExpr.getRight()));
    }
}

void CodeGen::visit(const ReturnStmt& ReturnStmt) {
    auto Ret = emitRValue(*ReturnStmt.getValue());
    Builder->CreateRet(Ret);

    const auto Block = emitBlock("return.after");
    Builder->SetInsertPoint(Block);
}

void CodeGen::visit(const IfStmt& IfStmt) {
    const auto CondBlock = emitBlock("if.cond");
    Builder->CreateBr(CondBlock);

    Builder->SetInsertPoint(CondBlock);
    const auto Cond = emitRValue(IfStmt.getCondition());

    auto ThenBlock = emitBlock("if.then");
    auto ElseBlock = emitBlock("if.else");

    Builder->CreateCondBr(Cond, ThenBlock, ElseBlock);
    Builder->SetInsertPoint(ThenBlock);
    IfStmt.getTrueBlock()->accept(*this);

    ThenBlock = Builder->GetInsertBlock();
    ElseBlock->moveAfter(ThenBlock);

    Builder->SetInsertPoint(ElseBlock);

    if (const auto FalseBlock = IfStmt.getFalseBlock()) {
        FalseBlock->accept(*this);
    }

    ElseBlock = Builder->GetInsertBlock();

    const auto FinishBlock = emitBlock("if.done");

    Builder->SetInsertPoint(ThenBlock);
    Builder->CreateBr(FinishBlock);
    Builder->SetInsertPoint(ElseBlock);
    Builder->CreateBr(FinishBlock);

    Builder->SetInsertPoint(FinishBlock);
}

void CodeGen::visit(const WhileStmt& WhileStmt) {
    auto CondBlock = emitBlock("while.cond");

    Builder->CreateBr(CondBlock);
    Builder->SetInsertPoint(CondBlock);

    const auto Cond = emitRValue(WhileStmt.getCondition());
    auto CondBr = Builder->CreateCondBr(Cond, CondBlock, CondBlock);

    const auto BodyBlock = emitBlock("while.body");
    Builder->SetInsertPoint(BodyBlock);
    WhileStmt.getBody().accept(*this);
    Builder->CreateBr(CondBlock);

    const auto FinishBlock = emitBlock("while.finish");
    Builder->SetInsertPoint(FinishBlock);

    CondBr->setSuccessor(0, BodyBlock);
    CondBr->setSuccessor(1, FinishBlock);
}

void CodeGen::visit(const FunctionCallExpr& FunctionCallExpr) {
    auto Func = llvm::cast<llvm::Function>(emitRValue(FunctionCallExpr.getFunction()));
    std::vector<llvm::Value*> Args;
    for (const auto &Arg : FunctionCallExpr.getArgs()) {
        const auto Value = emitRValue(*Arg);
        Value->getType()->dump();
        Args.push_back(Value);
    }
    returnValue(Builder->CreateCall(Func, Args));
}

void CodeGen::visit(const UnaryOpExpr& UnaryOpExpr) {
    const auto& Expr = UnaryOpExpr.getValue();
    switch (UnaryOpExpr.getKind()) {
        case TokenKind::Amp: {
            const auto Value = emitLValue(Expr);
            return returnValue(Value);
        }
            
    }
}

void CodeGen::visit(const AssignStmt& AssignStmt) {
    const auto Lhs = emitLValue(AssignStmt.getLeft());
    const auto Rhs = emitRValue(AssignStmt.getRight());

}

llvm::Function* CodeGen::emitFunctionProto(const FunctionDecl& FunctionDecl) {
    if (!NameValues.contains(&FunctionDecl)) {
        const auto FuncTy = llvm::cast<llvm::FunctionType>(TyEmitter->emit(*SemanInfo.getType(FunctionDecl)));
        const auto Func = llvm::Function::Create(FuncTy, llvm::GlobalValue::ExternalLinkage, FunctionDecl.getName(), TheModule.get());
        NameValues[&FunctionDecl] = Func;
    }
    return llvm::cast<llvm::Function>(NameValues[&FunctionDecl]);
}

llvm::BasicBlock* CodeGen::emitBlock(std::string_view Name = "") const {
    return llvm::BasicBlock::Create(*Context, Name, llvmFunction);
}

llvm::Value* CodeGen::emitLogicalAnd(const Expression& Left, const Expression& Right) {
    auto LhsBlock = emitBlock("land.lhs");
    Builder->CreateBr(LhsBlock);

    Builder->SetInsertPoint(LhsBlock);
    const auto Lft = emitRValue(Left);

    LhsBlock = Builder->GetInsertBlock();

    Builder->SetInsertPoint(emitBlock("land.rhs"));
    const auto Rgt = emitRValue(Right);

    const auto RhsBlock = Builder->GetInsertBlock();

    auto Finish = emitBlock("land.finish");

    Builder->SetInsertPoint(LhsBlock);
    Builder->CreateCondBr(Lft, RhsBlock, Finish);

    Builder->SetInsertPoint(RhsBlock);
    Builder->CreateBr(Finish);

    Builder->SetInsertPoint(Finish);
    auto Phi = Builder->CreatePHI(Builder->getInt1Ty(), 2);
    Phi->addIncoming(Builder->getFalse(), LhsBlock);
    Phi->addIncoming(Rgt, RhsBlock);
    return Phi;
}

llvm::Value* CodeGen::emitLogicalOr(const Expression& Left, const Expression& Right) {
    auto LhsBlock = emitBlock("lor.lhs");
    Builder->CreateBr(LhsBlock);

    Builder->SetInsertPoint(LhsBlock);
    const auto Lft = emitRValue(Left);

    LhsBlock = Builder->GetInsertBlock();

    Builder->SetInsertPoint(emitBlock("lor.lrhs"));
    const auto Rgt = emitRValue(Right);

    const auto RhsBlock = Builder->GetInsertBlock();

    auto Finish = emitBlock("lor.finish");

    Builder->SetInsertPoint(LhsBlock);
    Builder->CreateCondBr(Lft, Finish, RhsBlock);

    Builder->SetInsertPoint(RhsBlock);
    Builder->CreateBr(Finish);

    Builder->SetInsertPoint(Finish);
    auto Phi = Builder->CreatePHI(Builder->getInt1Ty(), 2);
    Phi->addIncoming(Builder->getTrue(), LhsBlock);
    Phi->addIncoming(Rgt, RhsBlock);
    return Phi;
}

llvm::Value* CodeGen::emitLValue(const Expression& Expr) {
    const auto Value = doVisit(Expr);
    return Value;
}

llvm::Value* CodeGen::emitRValue(const Expression& Expr) {
    const auto Value = doVisit(Expr);
    if (const auto Alloc = llvm::dyn_cast<llvm::AllocaInst>(Value)) {
        if (!Alloc->isArrayAllocation()) {
            return Builder->CreateLoad(Alloc->getAllocatedType(), Value);
        }
    }
    return Value;
}
