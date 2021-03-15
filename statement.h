#ifndef STATEMENT_H
#define STATEMENT_H

#include "exp.h"
#include <QTextBrowser>

/*
 * Type: StatementType
 * -------------------
 * This enumerated type is used to differntiate the seven different
 * statement types: RemStmt, LetStmt, PrintStmt, InputSymt, GotoStmt,
 * IfStmt, EndStmt.
 */

enum StatementType { RemStmt, LetStmt, PrintStmt, InputStmt,
                     GotoStmt, IfStmt, EndStmt };

/*
 * Class: Statement
 * ----------------
 * This class is used to encapsulate a single statement in program.
 * Every Statement object is created using one of the seven concrete
 * subclasses shown above.
 */
class Statement {
public:
    Statement();
    virtual ~Statement();
    virtual StatementType getType() = 0;
    virtual int execute(EvaluationContext &programContext) = 0; // -1: error, 0: normal, n: jump to n;
    virtual QString toString() = 0;
protected:
    Expression *getExp(QVector<Token> tokens);
    int getPre(QString mark);
    bool isJudge(QString mark);
};

class RemStatement: public Statement {
public:
    RemStatement(QVector<Token> tokens);
    virtual StatementType getType();
    virtual int execute(EvaluationContext &programContext);
    virtual QString toString();
private:
    QString remText;
};

class LetStatement: public Statement {
private:
    QString variableName;
    Expression* rightExp;
public:
    LetStatement(QVector<Token> tokens);
    virtual ~LetStatement();
    virtual StatementType getType();
    virtual int execute(EvaluationContext &programContext);
    virtual QString toString();
};

class PrintStatement: public Statement {
private:
    Expression* printExp;
    QTextBrowser* res_display;
public:
    PrintStatement(QVector<Token> tokens, QTextBrowser* rd);
    virtual ~PrintStatement();
    virtual StatementType getType();
    virtual int execute(EvaluationContext &programContext);
    virtual QString toString();
};

class InputStatement: public Statement {
private:
    QString variableName;
public:
    InputStatement(QVector<Token> tokens);
    virtual StatementType getType();
    virtual int execute(EvaluationContext &programContext);
    virtual QString toString();
};

class GotoStatement: public Statement {
private:
    int gotoLineNum;
public:
    GotoStatement(QVector<Token> tokens);
    virtual StatementType getType();
    virtual int execute(EvaluationContext &progCont);
    virtual QString toString();
};

class IfStatement: public Statement {
private:
    Expression *exp1;
    Expression *exp2;
    QString op;
    int targetN;
public:
    IfStatement(QVector<Token> tokens);
    virtual StatementType getType();
    virtual int execute(EvaluationContext &progCont);
    virtual QString toString();
};

class EndStatement: public Statement {
public:
    EndStatement();
    virtual StatementType getType();
    virtual int execute(EvaluationContext &progCont);
    virtual QString toString();
};

#endif // STATEMENT_H
