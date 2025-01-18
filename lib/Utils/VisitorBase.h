#pragma once

template <typename Derived, typename VisitableType, typename RetType>
class VisitorBase {
public:
    RetType doVisit(VisitableType& Ty) {
        Ty.accept(static_cast<Derived&>(*this));
        return Result;
    }
    void returnValue(RetType Ret) { Result = Ret; }
private:
    RetType Result;
};