#include "exp.h"
#include <cmath>

QString SyntaxTree::getPrintStr(int layer) {
    QString str = QString(layer * 4, ' ') + cur_string + "\n";
    if (left) str += left->getPrintStr(layer + 1);
    if (right) str += right->getPrintStr(layer + 1);
    return str;
}

Expression::Expression() {}

Expression::~Expression() {}

int Expression::getConstantValue() {
    error("Calling unimplemented method from wrong class object.");
    return 0;
}

QString Expression::getIdentifierName() {
    error("Calling unimplemented method from wrong class object.");
    return "\0";
}

QString Expression::getOperator() {
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

SyntaxTree *Expression::getSyntaxTree() {
    error("Calling unimplemented method from wrong class object.");
    return nullptr;
}

ConstantExp::ConstantExp(int val): value(val) {}

int ConstantExp::eval(EvaluationContext & ) {
    return value;
}

QString ConstantExp::toString() {
    return QString::number(value);
}

ExpressionType ConstantExp::type() {
    return CONSTANT;
}

int ConstantExp::getConstantValue() {
    return value;
}

SyntaxTree *ConstantExp::getSyntaxTree() {
    return new SyntaxTree(QString::number(value));
}

IdentifierExp::IdentifierExp(QString _name): name(_name) {}

int IdentifierExp::eval(EvaluationContext & context) {
    if (!context.isDefined(name)) {
        error(name.toStdString() + " is undefined");
    }
    return context.getValue(name);
}

QString IdentifierExp::toString() {
    return name;
}

ExpressionType IdentifierExp::type() {
    return IDENTIFIER;
}

QString IdentifierExp::getIdentifierName() {
    return name;
}

SyntaxTree *IdentifierExp::getSyntaxTree() {
    return new SyntaxTree(name);
}

CompoundExp::CompoundExp(QString _op, Expression *_lhs, Expression *_rhs):
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
       if (op == "**") return pow(left, right);
       error("Illegal operator in expression");
       return 0;
}

QString CompoundExp::toString() {
    return lhs->toString() + " " + op + " " + rhs->toString();
}

ExpressionType CompoundExp::type() {
    return COMPOUND;
}

QString CompoundExp::getOperator() {
    return op;
}

Expression* CompoundExp::getLHS() {
    return lhs;
}

Expression* CompoundExp::getRHS() {
    return rhs;
}

SyntaxTree *CompoundExp::getSyntaxTree() {
    SyntaxTree *l = lhs->getSyntaxTree();
    SyntaxTree *r = rhs->getSyntaxTree();
    return new SyntaxTree(op, l, r);
}
