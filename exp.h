/*
 * File: exp.h
 * -----------
 * This interface defines a class hierarchy for arithmetic expressions.
 */

#ifndef EXP_H
#define EXP_H

#include <QString>
#include <map>
#include <cstdlib>
#include "tokenizer.h"
#include "evalstate.h"
#include "errorhandler.h"

/* Forward reference */

class EvaluationContext;

class SyntaxTree {
public:
    QString cur_string;
    SyntaxTree *left;
    SyntaxTree *right;

    SyntaxTree(QString s, SyntaxTree *l = nullptr, SyntaxTree *r = nullptr) {
        cur_string = s;
        left = l;
        right = r;
    }

    QString getPrintStr(int layer);
};

/*
 * Type: ExpressionType
 * --------------------
 * This enumerated type is used to differentiate the three different
 * expression types: CONSTANT, IDENTIFIER, and COMPOUND.
 */

enum ExpressionType { CONSTANT, IDENTIFIER, COMPOUND, STRING, STRINGIDENTIFIER };

enum IdentifierType { INT, STR };

/*
 * Class: Expression
 * -----------------
 * This class is used to represent a node in an expression tree.
 * Expression itself is an abstract class.  Every Expression object
 * is therefore created using one of the three concrete subclasses:
 * ConstantExp, IdentifierExp, or CompoundExp.
 */

class Expression {

public:

   Expression();
   virtual ~Expression();
   virtual int eval(EvaluationContext & context) = 0;
   virtual QString toString() = 0;
   virtual ExpressionType type() = 0;

/* Getter methods for convenience */

   virtual int getConstantValue();
   virtual QString getIdentifierName();
   virtual QString getOperator();
   virtual Expression *getLHS();
   virtual Expression *getRHS();
   virtual SyntaxTree *getSyntaxTree();
   virtual QString evalString(EvaluationContext &);

};

/*
 * Class: ConstantExp
 * ------------------
 * This subclass represents a constant integer expression.
 */

class ConstantExp: public Expression {

public:

   ConstantExp(int val);
   ConstantExp(int val, bool p);

   virtual int eval(EvaluationContext & context);
   virtual QString toString();
   virtual ExpressionType type();

   virtual int getConstantValue();
   virtual SyntaxTree *getSyntaxTree();

private:
    bool isPrefix0;
    int value;

};

/*
 * Class: IdentifierExp
 * --------------------
 * This subclass represents a expression corresponding to a variable.
 */

class IdentifierExp: public Expression {

public:

   IdentifierExp(QString name);

   virtual int eval(EvaluationContext & context);
   virtual QString toString();
   virtual ExpressionType type();

   virtual QString getIdentifierName();
   virtual SyntaxTree *getSyntaxTree();

private:

   QString name;

};

/*
 * Class: CompoundExp
 * ------------------
 * This subclass represents a compound expression.
 */

class CompoundExp: public Expression {

public:

   CompoundExp(QString op, Expression *lhs, Expression *rhs);
   virtual ~CompoundExp();

   virtual int eval(EvaluationContext & context);
   virtual QString toString();
   virtual ExpressionType type();

   virtual QString getOperator();
   virtual Expression *getLHS();
   virtual Expression *getRHS();
   virtual SyntaxTree *getSyntaxTree();

private:

   QString op;
   Expression *lhs, *rhs;

};

//class StringIdentifierExp: public Expression {

//public:

//    StringIdentifierExp(QString _name);
//    virtual ~StringIdentifierExp();

//    virtual ExpressionType type();
//    virtual QString toString();
//    virtual QString evalString(EvaluationContext & context);
//    virtual SyntaxTree *getSyntaxTree();

//private:

//    QString name;
//};

class StringExp: public Expression {

public:

    StringExp(QString _value);
    virtual ~StringExp();

    virtual ExpressionType type();
    virtual QString toString();
    virtual int eval(EvaluationContext & context);
    virtual SyntaxTree *getSyntaxTree();

private:

    QString value;
};

#endif
