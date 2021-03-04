#include "exp.h"

ConstantExp::ConstantExp(int val): value(val) {}

int ConstantExp::eval(EvaluationContext & ) {
    return value;
}

int IdentifierExp::eval(EvaluationContext & context) {
    if (!context.isDefined(name)) {
        error(name + " is undefined");
    }
    return context.getValue(name);
}

int CompoundExp::eval(EvaluationContext & context) {
    int right = rhs->eval(context);
       if (op == "=") {
          context.setValue(lhs->getIdentifierName(), right);
          return right;
       }
       int left = lhs->eval(context);
       if (op == "+") return left + right;
       if (op == "-") return left - right;
       if (op == "*") return left * right;
       if (op == "/") {
          if (right == 0) error("Division by 0");
          return left / right;
       }
       error("Illegal operator in expression");
       return 0;
}

std::string ConstantExp::toString() {
    return std::to_string(value);
}

ExpressionType ConstantExp::type() {
    return CONSTANT;
}

int ConstantExp::getConstantValue() {
    return value;
}
