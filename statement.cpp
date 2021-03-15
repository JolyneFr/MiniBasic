#include "statement.h"
#include <QtDebug>
#include <QInputDialog>
#include <QStack>

bool isBeginWord(StringType st) {
    if (st < THEN) {
        return true;
    }
    return false;
}

Statement::Statement() {}

Statement::~Statement() {}

StatementType Statement::getType() {
    error("Calling statement function from base class.");
    return StatementType(0);
}

int Statement::execute(EvaluationContext&) {
    error("Calling statement function from base class.");
    return -1;
}

Expression *Statement::getExp(QVector<Token> tokens) {
    QStack<QString> Operators;
    QStack<Expression*> Operands;
    QVector<Token>::Iterator cur_token = tokens.begin();
    while (cur_token != tokens.end()) {
        if (cur_token->getType() == Number) {
            Operands.push(new ConstantExp(cur_token->getNumber()));
        } else if (cur_token->getType() == String) {
            Operands.push(new IdentifierExp(cur_token->toString()));
        } else { //cur_token->getType() == Mark
            QString cur_mark = cur_token->getOp();
            if (cur_mark == "(") {
                Operators.push(cur_mark);
            } else if (cur_mark != ")") {
                while (!Operators.empty() && getPre(cur_mark) <= getPre(Operators.top())) {
                    Expression *right = Operands.pop();
                    Expression *left = Operands.pop();
                    QString curOp = Operators.pop();
                    Operands.push(new CompoundExp(curOp, left, right));
                }
                Operators.push(cur_mark);
            } else { // cur_mark == ")"
                while (Operators.top() != "(") {
                    Expression *right = Operands.pop();
                    Expression *left = Operands.pop();
                    QString curOp = Operators.pop();
                    Operands.push(new CompoundExp(curOp, left, right));
                }
                Operators.pop();
            }
        }
        cur_token++;
    }
    while (Operands.size() > 1) {
        Expression *right = Operands.pop();
        Expression *left = Operands.pop();
        QString curOp = Operators.pop();
        Operands.push(new CompoundExp(curOp, left, right));
    }
    return Operands[0];
}

int Statement::getPre(QString mark) {
    if (mark == "**") return 4;
    if (mark == "*" || mark == "/") return 3;
    if (mark == "+" || mark == "-") return 2;
    if (mark == "=") return 1;
    return 0;
}

bool Statement::isJudge(QString mark) {
    if (mark == "<" || mark == "=" || mark == ">")
        return true;
    return false;
}

RemStatement::RemStatement(QVector<Token> rems) {
    QString rem_text = "";
    for (Token cur_token : rems) {
        remText += cur_token.toString() + " ";
    }
    remText = rem_text.trimmed();
}

StatementType RemStatement::getType() { return RemStmt; }

int RemStatement::execute(EvaluationContext&) { return 0; }

QString RemStatement::toString() {
    return "REM " + remText;
}

LetStatement::LetStatement(QVector<Token> tks) {
    variableName = tks[0].toString();
    tks.pop_front();
    tks.pop_front();
    rightExp = getExp(tks);
}

LetStatement::~LetStatement() { rightExp->~Expression(); }

StatementType LetStatement::getType() { return LetStmt; }

int LetStatement::execute(EvaluationContext &programContext) {
    programContext.setValue(variableName, rightExp->eval(programContext));
    return 0;
}

QString LetStatement::toString() {
    return "LET " + variableName + " = " + rightExp->toString();
}

PrintStatement::PrintStatement(QVector<Token> tks, QTextBrowser* rd): res_display(rd) {
    printExp = getExp(tks);
}

PrintStatement::~PrintStatement() { printExp->~Expression(); }

StatementType PrintStatement::getType() { return PrintStmt; }

int PrintStatement::execute(EvaluationContext &programContext) {
    res_display->insertPlainText(QString::number(printExp->eval(programContext)) + '\n');
    return 0;
}

QString PrintStatement::toString() {
    return "PRINT " + printExp->toString();
}

InputStatement::InputStatement(QVector<Token> tks) {
    if (tks.length() == 1 && tks[0].getType() == String &&
            tks[0].getWordType() == Variable) {
        variableName = tks[0].toString();
    } else {
        error("Invalid Input Statment Format!");
    }
}

StatementType InputStatement::getType() { return InputStmt; }

int InputStatement::execute(EvaluationContext &programContext) {
    int inputNum = QInputDialog::getInt(NULL, "Input Dialog", "Please input " + variableName + ":");
    programContext.setValue(variableName, inputNum);
    return 0;
}

QString InputStatement::toString() {
    return "INPUT " + variableName;
}

GotoStatement::GotoStatement(QVector<Token> tks) {
    if (tks.size() != 1 || tks[0].getType() != Number)
        error("Target line should be a constant.");
    gotoLineNum = tks[0].getNumber();
}

StatementType GotoStatement::getType() { return GotoStmt; }

int GotoStatement::execute(EvaluationContext &) {
    return gotoLineNum;
}

QString GotoStatement::toString() {
    return "GOTO " + QString::number(gotoLineNum);
}

IfStatement::IfStatement(QVector<Token> tokens) {
    QVector<Token>::Iterator find_itr1 = tokens.begin();
    while (find_itr1 != tokens.end() && !isJudge(find_itr1->toString())) {
        find_itr1++;
    }
    QVector<Token> expTokens1(tokens.begin(), find_itr1);
    exp1 = getExp(expTokens1);
    op = find_itr1->toString();
    find_itr1++;
    QVector<Token>::Iterator find_itr2 = find_itr1;
    while (find_itr2 != tokens.end() && find_itr2->getWordType() != THEN) {
        find_itr2++;
    }
    QVector<Token> expTokens2(find_itr1, find_itr2);
    exp2 = getExp(expTokens2);
    find_itr2++;
    if (find_itr2 + 1 == tokens.end() && find_itr2->getType() == Number) {
        targetN = find_itr2->getNumber();
    } else {
        error("Wrong grammar in IfStatement.");
    }
}

StatementType IfStatement::getType() { return IfStmt; };

int IfStatement::execute(EvaluationContext &programContext) {
    int left = exp1->eval(programContext);
    int right = exp2->eval(programContext);
    bool ifJump = false;
    if (op == "<") ifJump = (left < right);
    if (op == "=") ifJump = (left == right);
    if (op == ">") ifJump = (left > right);
    if (ifJump) return targetN;
    return 0;
}

QString IfStatement::toString() {
    return "IF " + exp1->toString() + " " + op + " " +
            exp2->toString() + " THEN " + QString::number(targetN);
}

EndStatement::EndStatement() {}

StatementType EndStatement::getType() { return EndStmt; }

int EndStatement::execute(EvaluationContext &) { return -1; }

QString EndStatement::toString() { return "END"; }
