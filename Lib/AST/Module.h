#pragma once
#include "ASTBase.h"
#include "Decl.h"
#include <vector>

class Module : public AstBase {
private:
    std::vector<AstPtr<Declaration>> Declarations;
};
