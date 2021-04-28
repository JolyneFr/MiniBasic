#ifndef STATEMENT_H
#define STATEMENT_H

#include "exp.h"
#include <QTextBrowser>
#include <QLabel>

class StatementTree {
public:
    QString rootString;
    SyntaxTree **childs;
    int childN;

    StatementTree(QString rs, SyntaxTree **cs, int n):
        rootString(rs), childs(cs), childN(n) {}

    QString getSyntaxStr(int lineN);
};

/*
 * Type: StatementType
 * -------------------
 * This enumerated type is used to differntiate the seven different
 * statement types: RemStmt, LetStmt, PrintStmt, InputSymt, GotoStmt,
 * IfStmt, EndStmt.
 */

enum StatementType { RemStmt, LetStmt, PrintStmt, InputStmt,
                     GotoStmt, IfStmt, EndStmt, ErrorStmt };

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
    virtual StatementTree *getTree() = 0;
protected:
    QVector<Token> getTokens;
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
    virtual StatementTree *getTree();
private:
    QString text;
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
    virtual StatementTree *getTree();
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
    virtual StatementTree *getTree();
};

class InputStatement: public Statement {
private:
    QString variableName;
public:
    InputStatement(QVector<Token> tokens);
    virtual StatementType getType();
    virtual int execute(EvaluationContext &programContext);
    virtual QString toString();
    virtual StatementTree *getTree();
};

class GotoStatement: public Statement {
private:
    int gotoLineNum;
public:
    GotoStatement(QVector<Token> tokens);
    virtual StatementType getType();
    virtual int execute(EvaluationContext &progCont);
    virtual QString toString();
    virtual StatementTree *getTree();
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
    virtual StatementTree *getTree();
};

class EndStatement: public Statement {
public:
    EndStatement();
    virtual StatementType getType();
    virtual int execute(EvaluationContext &progCont);
    virtual QString toString();
    virtual StatementTree *getTree();
};

class ErrorStatement: public Statement {
public:
    ErrorStatement(QString msg);
    virtual StatementType getType();
    virtual int execute(EvaluationContext &progCont);
    virtual QString toString();
    virtual StatementTree *getTree();
private:
    QString text;
    QString errorMessage;
};

#endif // STATEMENT_H
