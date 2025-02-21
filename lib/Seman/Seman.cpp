#include "Seman.h"
#include "Validator.h"
#include "NameResolver.h"
#include "TypeCheck.h"


Seman::Seman(TypeContext& TyContext, ErrorReporter& Reporter) : TyContext(TyContext), Reporter(Reporter) {
}

void Seman::visit(Module& Module) {
    CurrentSource = &Module.getSourceFile();
    Validator Validator(*this, TyContext);
    if (Validator.doVisit(Module)) {
        return;
    }
    const auto& StructTypes = Validator.getStructTypes();
    NameResolver Resolver(*this, StructTypes);
    Module.accept(Resolver);
    TypeCheck TyCheck(*this);
    TyCheck.doVisit(Module);
}
