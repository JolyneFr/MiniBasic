#include "statement.h"
#include <QtDebug>
#include <QInputDialog>
#include <QStack>
#include <QQueue>

QString StatementTree::getSyntaxStr(int lineN) {
    QString printStr = QString::number(lineN) + " " + rootString + "\n";
    for (int index = 0; index < childN; index++) {
        SyntaxTree *child = childs[index];
        printStr += child->getPrintStr(1);
    }
    return printStr;
}

bool isBeginWord(StringType st) {
    if (st < THEN) {
        return true;
    }
    return false;
}

bool isOperator(QString s) {
    if (s == "-" || s == "+" || s == "*" || s == "/" || s == "**")
        return true;
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

StatementTree *Statement::getTree() {
    error("Calling statement function from base class.");
    return nullptr;
}

Expression *Statement::getExp(QVector<Token> tokens) {

    // judge StringExp first
    if (tokens.empty()) error("Expect expression.");

    QStack<QString> Operators;
    QStack<Expression*> Operands;
    QVector<Token>::Iterator cur_token = tokens.begin();
    if (cur_token == tokens.end())
        error("Empty expression. Check your syntax.");
    while (cur_token != tokens.end()) {
        if (cur_token->getType() == Number) {
            Operands.push(new ConstantExp(cur_token->getNumber()));
        } else if (cur_token->getType() == String) {
            if (cur_token->getWordType() == Variable)
                Operands.push(new IdentifierExp(cur_token->toString()));
            else
                error("Invalid variable name.");
        } else { //cur_token->getType() == Mark
            QString cur_mark = cur_token->getOp();
            if (cur_mark == "(" || cur_mark == "**") {
                Operators.push(cur_mark);
            } else if (cur_mark != ")") {
                while (!Operators.empty() && !Operands.empty() && getPre(cur_mark) <= getPre(Operators.top())) {
                    if (Operands.size() < 2)
                        error("Invalid Syntax! Please Check Your Input.");
                    Expression *right = Operands.pop();
                    Expression *left = Operands.pop();
                    QString curOp = Operators.pop();
                    if (!isOperator(curOp))
                        error("Mismatched bracket");
                    Operands.push(new CompoundExp(curOp, left, right));
                }
                Operators.push(cur_mark);
            } else { // cur_mark == ")"
                while (Operators.top() != "(") {
                    if (Operands.empty() || Operators.empty())
                        error("Missing '('. Check your expression input.");
                    Expression *right = Operands.pop();
                    QString curOp = Operators.pop();
                    if (!isOperator(curOp))
                        error("Mismatched bracket");
                    if (Operands.empty())
                        error("Missing left operand.");
                    Expression *left = Operands.pop();
                    Operands.push(new CompoundExp(curOp, left, right));
                }
                if (Operators.empty())
                    error("Missing '('. Check your expression input.");
                Operators.pop();
            }
        }
        cur_token++;
    }
    while (Operands.size() > 1) {
        if (Operators.empty())
            error("Missing operator. Check your expression input.");
        QString curOp = Operators.pop();
        Expression *right = Operands.pop();
        Expression *left = Operands.pop();
        if (!isOperator(curOp))
            error("Mismatched bracket");
        Operands.push(new CompoundExp(curOp, left, right));
    }
    if (!Operators.empty()) {
        if (Operators.size() == 1 && Operators.top() == "-") { // prefix "-"
            QString curOp = Operators.pop();
            Expression *right = Operands.pop();
            Expression *left = new ConstantExp(0, true);
            Operands.push(new CompoundExp(curOp, left, right));
        } else {
            error("Mismatched operator.");
        }
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
    getTokens = rems;
    QString rem_text = "";
    for (Token cur_token : getTokens) {
        rem_text += cur_token.toString() + " ";
    }
    text = rem_text.trimmed();
}

StatementType RemStatement::getType() { return RemStmt; }

int RemStatement::execute(EvaluationContext&) { return 0; }

QString RemStatement::toString() {
    return "REM " + text;
}

StatementTree *RemStatement::getTree() {
    SyntaxTree **childs = new SyntaxTree*[1];
    SyntaxTree *rem = new SyntaxTree(text);
    childs[0] = rem;
    return new StatementTree("REM", childs, 1);
}

LetStatement::LetStatement(QVector<Token> tks) {
    getTokens = tks;
    if (tks[0].getType() != String ||
        tks[0].getWordType() != Variable)
        error("Wrong format of variable.");
    if (tks.size() <= 2 || tks[1].toString() != "=")
        error("Expect '=' in LET statement.");
    variableName = tks[0].toString();
    tks.pop_front();
    tks.pop_front();
    if (tks[0].toString() == "\"") {
        if (tks.size() != 3 || tks[2].toString() != "\"")
            error("Illegal format of string!");
        rightExp = new StringExp(tks[1].toString());
    } else if (tks[0].toString() == "'") {
        if (tks.size() != 3 || tks[2].toString() != "'")
            error("Illegal format of string!");
        rightExp = new StringExp(tks[1].toString());
    } else rightExp = getExp(tks);
}

LetStatement::~LetStatement() { rightExp->~Expression(); }

StatementType LetStatement::getType() { return LetStmt; }

int LetStatement::execute(EvaluationContext &programContext) {
    if (rightExp->type() == STRING) {
        programContext.setValue(variableName, rightExp->toString());
    } else {
        programContext.setValue(variableName, rightExp->eval(programContext));
    }
    return 0;
}

QString LetStatement::toString() {
    QString _text = "LET ";
    for (Token cur_token : getTokens) {
        _text += cur_token.toString() + " ";
    }
    return _text.trimmed();
}

StatementTree *LetStatement::getTree() {
    SyntaxTree **childs = new SyntaxTree*[2];
    childs[0] = new SyntaxTree(variableName); //left;
    childs[1] = rightExp->getSyntaxTree(); //right
    return new StatementTree("LET =", childs, 2);
}

PrintStatement::PrintStatement(QVector<Token> tks, QTextBrowser* rd): res_display(rd) {
    getTokens = tks;
    printExp = getExp(tks);
}

PrintStatement::~PrintStatement() { printExp->~Expression(); }

StatementType PrintStatement::getType() { return PrintStmt; }

int PrintStatement::execute(EvaluationContext &programContext) {
    res_display->insertPlainText(QString::number(printExp->eval(programContext)) + '\n');
    return 0;
}

QString PrintStatement::toString() {
    QString _text = "PRINT ";
    for (Token cur_token : getTokens) {
        _text += cur_token.toString() + " ";
    }
    return _text.trimmed();
}

StatementTree *PrintStatement::getTree() {
    SyntaxTree **childs = new SyntaxTree*[1];
    SyntaxTree *t = printExp->getSyntaxTree();
    childs[0] = t;
    return new StatementTree("PRINT", childs, 1);
}

PrintfStatement::PrintfStatement(QVector<Token> tokens, QTextBrowser* rd): res_display(rd) {
    if (tokens.empty())
        error("empty statement is not supported!");
    getTokens = tokens;
    paramNumber = 0;
    int index;
    fmtString = "";
    if (tokens[0].toString() == "\"") {
        if (tokens.size() < 3 || tokens[2].toString() != "\"") {
            error("wrong format!");
        }
        fmtString = "\"" + tokens[1].toString() + "\"";
        index = 3;
    } else if (tokens[0].toString() == "'") {
        if (tokens.size() < 3 || tokens[2].toString() != "'") {
            error("wrong format!");
        }
        fmtString = "'" + tokens[1].toString() + "'";
        index = 3;
    } else if (tokens[0].getType() == String && tokens[0].getWordType() == Variable) {
        fmtString = tokens[0].toString();
        index = 1;
    } else {
        error("wrong format!");
        index = 0;
    }

    paramNumber = 0;
    while (index < tokens.size()) {
        if (tokens[index++].toString() == ",") {
            if (index >= tokens.size())
                error("wrong format");
            if (tokens[index].toString() == "\"") {
                index++;
                fmtTable.append(str);
                if (index >= tokens.size())
                    error("wrong format");
                paramStrings.push_back(tokens[index++].toString());
                if (index >= tokens.size() || tokens[index++].toString() != "\"") {
                    error("Unclosed '\"' charactor.");
                }
            } else if (tokens[index].toString() == "'") {
                index++;
                fmtTable.append(str);
                if (index >= tokens.size())
                    error("wrong format");
                paramStrings.push_back(tokens[index++].toString());
                if (index >= tokens.size() || tokens[index++].toString() != "'") {
                    error("Unclosed ''' charactor.");
                }
            } else {
                fmtTable.append(exp);
                QVector<Token> exp_tokens;
                while (index < tokens.size() && tokens[index].toString() != ",") {
                    exp_tokens.append(tokens[index]);
                    index++;
                }
                paramExps.append(getExp(exp_tokens));
            }
            paramNumber++;
        } else {
            error("There should be ',' before each param.");
        }
    }
}

PrintfStatement::~PrintfStatement() {}

StatementType PrintfStatement::getType() {
    return PrintfStmt;
}

QString PrintfStatement::getRootString() {
    return fmtString;
}

StatementTree *PrintfStatement::getTree() {
    int childNum = paramStrings.size() + paramExps.size();
    SyntaxTree **childs = new SyntaxTree*[childNum];
    int expIndex = 0, strIndex = 0, allIndex = 0;
    for (fmtType t : fmtTable) {
        if (t != fmt) {
            SyntaxTree *cur_child;
            if (t == exp) {
                cur_child = paramExps[expIndex++]->getSyntaxTree();
            } else {
                cur_child = new SyntaxTree("\"" + paramStrings[strIndex++] + "\"");
            }
            childs[allIndex++] = cur_child;
        }
    }
    return new StatementTree("PRINTF " + getRootString(), childs, childNum);
}

int PrintfStatement::execute(EvaluationContext &context) {

    QString format;
    if (fmtString[0] == '"' || fmtString[0] == '\'') {
        format = fmtString.mid(1, fmtString.size() - 2);
    } else {
        format = context.getString(fmtString);
    }

    QVector<Token> fmt_tokens = parse_format(format);
    int tmp_param_number = 0, i = 0;
    while (i < fmt_tokens.size()) {
        if (fmt_tokens[i].toString() == "{}") {
            tmp_param_number++;
        }
        i++;
    }
    if (tmp_param_number != paramNumber)
        error("The number of parameters does not match ");

    QString output = "";
    int varIndex = 0, strIndex = 0, fmtIndex = 0;
    for (Token t : fmt_tokens) {
        if (t.toString() == "{}") {
            fmtType cur_fmt = fmtTable[fmtIndex++];
            if (cur_fmt == exp) {
                Expression *cur_exp = paramExps[varIndex++];
                if (cur_exp->type() != IDENTIFIER) {
                    output.append(QString::number(cur_exp->eval(context)));
                } else {
                    QString name = cur_exp->getIdentifierName();
                    if (context.isDefinedInt(name)) {
                        output += QString::number(context.getValue(name));
                        continue;
                    }
                    if (context.isDefinedString(name)) {
                        output += context.getString(name);
                        continue;
                    }
                    error(("Param " + name + " is undefined.").toStdString());
                }
            } else {// str
                output += paramStrings[strIndex++];
            }
        } else {
            output += t.toString();
        }
    }

    res_display->insertPlainText(output + '\n');
    return 0;
}

QString PrintfStatement::toString() {
    QString _text = "PRINTF ";
    for (Token cur_token : getTokens) {
        _text += cur_token.toString() + " ";
    }
    return _text.trimmed();
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

StatementTree *InputStatement::getTree() {
    SyntaxTree **childs = new SyntaxTree*[1];
    SyntaxTree *t = new SyntaxTree(variableName);
    childs[0] = t;
    return new StatementTree("INPUT", childs, 1);
}

InputsStatement::InputsStatement(QVector<Token> tks) {
    if (tks.length() == 1 && tks[0].getType() == String &&
            tks[0].getWordType() == Variable) {
        variableName = tks[0].toString();
    } else {
        error("Invalid Input Statment Format!");
    }
}

StatementType InputsStatement::getType() {
    return InputsStmt;
}

int InputsStatement::execute(EvaluationContext &programContext) {
    QString text = QInputDialog::getText(NULL, "Input Dialog", "Please input " + variableName + ":");
    programContext.setValue(variableName, text);
    return 0;
}

QString InputsStatement::toString() {
    return "INPUTS " + variableName;
}

StatementTree *InputsStatement::getTree() {
    SyntaxTree **childs = new SyntaxTree*[1];
    SyntaxTree *t = new SyntaxTree(variableName);
    childs[0] = t;
    return new StatementTree("INPUTS", childs, 1);
}

GotoStatement::GotoStatement(QVector<Token> tks) {
    if (tks.length() != 1)
        error("Invalid GOTO statement format.");
    if (tks[0].getType() != Number)
        error("Target line should be a constant.");
    gotoLineNum = tks[0].getNumber();
    if (gotoLineNum <= 0 || gotoLineNum > 1000000)
        error("Invalid goto target number. Too big or too small.");
}

StatementType GotoStatement::getType() { return GotoStmt; }

int GotoStatement::execute(EvaluationContext &) {
    return gotoLineNum;
}

QString GotoStatement::toString() {
    return "GOTO " + QString::number(gotoLineNum);
}

StatementTree *GotoStatement::getTree() {
    SyntaxTree **childs = new SyntaxTree*[1];
    SyntaxTree *t = new SyntaxTree(QString::number(gotoLineNum));
    childs[0] = t;
    return new StatementTree("GOTO", childs, 1);
}

IfStatement::IfStatement(QVector<Token> tokens) {
    getTokens = tokens;
    QVector<Token>::Iterator find_itr1 = tokens.begin();
    while (find_itr1 != tokens.end() && !isJudge(find_itr1->toString())) {
        find_itr1++;
    }
    if (find_itr1 == tokens.end())
        error("Expect compare symbol such as '<', '=' and '>'.");
    QVector<Token> expTokens1(tokens.begin(), find_itr1);
    exp1 = getExp(expTokens1);
    op = find_itr1->toString();
    find_itr1++;
    QVector<Token>::Iterator find_itr2 = find_itr1;
    while (find_itr2 != tokens.end() && find_itr2->getWordType() != THEN) {
        find_itr2++;
    }
    if (find_itr2 == tokens.end())
        error("Expect 'THEN' word.");
    QVector<Token> expTokens2(find_itr1, find_itr2);
    exp2 = getExp(expTokens2);
    find_itr2++;
    if (find_itr2 == tokens.end())
        error("Expect target LineNumber");
    if (find_itr2 + 1 == tokens.end() && find_itr2->getType() == Number) {
        targetN = find_itr2->getNumber();
        if (targetN <= 0 || targetN > 1000000)
            error("Invalid target number. Too big or too small.");
    } else {
        error("Statement too long or wrong target format.");
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
    QString _text = "IF ";
    for (Token cur_token : getTokens) {
        _text += cur_token.toString() + " ";
    }
    return _text.trimmed();;
}

StatementTree *IfStatement::getTree() {
    SyntaxTree **childs = new SyntaxTree*[4];
    childs[0] = exp1->getSyntaxTree();
    childs[1] = new SyntaxTree(op);
    childs[2] = exp2->getSyntaxTree();
    childs[3] = new SyntaxTree(QString::number(targetN));
    return new StatementTree("IF THEN", childs, 4);
}

EndStatement::EndStatement() {}

StatementType EndStatement::getType() { return EndStmt; }

int EndStatement::execute(EvaluationContext &) { return -2; }

QString EndStatement::toString() { return "END"; }

StatementTree *EndStatement::getTree() {
    return new StatementTree("END", nullptr, 0);
}

ErrorStatement::ErrorStatement(QString msg): errorMessage(msg) {}

StatementType ErrorStatement::getType() { return ErrorStmt; }

int ErrorStatement::execute(EvaluationContext &) {
    error("Executing error statement with problem: " + errorMessage.toStdString());
    return -1;
}

QString ErrorStatement::toString() { return "ERROR"; }

StatementTree *ErrorStatement::getTree() {
    return new StatementTree("Error", nullptr, 0);
}
