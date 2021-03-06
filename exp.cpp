#include "exp.h"

Expression::Expression() {}

Expression::~Expression() {}

int Expression::getConstantValue() {
    error("Calling unimplemented method from wrong class object.");
    return 0;
}

std::string Expression::getIdentifierName() {
    error("Calling unimplemented method from wrong class object.");
    return "\0";
}

std::string Expression::getOperator() {
    error("Calling unimplemented method from wrong class object.");
    return "\0";
}

Expression* Expression::getLHS() {
    error("Calling unimplemented method from wrong class object.");
    return nullptr;
}

Expression* Expression::getRHS() {
    error("Calling unimplemented method from wrong class object.");
    return nullptr;
}

ConstantExp::ConstantExp(int val): value(val) {}

int ConstantExp::eval(EvaluationContext & ) {
    return value;
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

IdentifierExp::IdentifierExp(std::string _name): name(_name) {}

int IdentifierExp::eval(EvaluationContext & context) {
    if (!context.isDefined(name)) {
        error(name + " is undefined");
    }
    return context.getValue(name);
}

std::string IdentifierExp::toString() {
    return name;
}

ExpressionType IdentifierExp::type() {
    return IDENTIFIER;
}

std::string IdentifierExp::getIdentifierName() {
    return name;
}

CompoundExp::CompoundExp(std::string _op, Expression *_lhs, Expression *_rhs):
    op(_op), lhs(_lhs), rhs(_rhs) {}

CompoundExp::~CompoundExp() {
    lhs->~Expression();
    rhs->~Expression();
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

std::string CompoundExp::toString() {
    return lhs->toString() + op + rhs->toString();
}

ExpressionType CompoundExp::type() {
    return COMPOUND;
}

std::string CompoundExp::getOperator() {
    return op;
}

Expression* CompoundExp::getLHS() {
    return lhs;
}

Expression* CompoundExp::getRHS() {
    return rhs;
}

