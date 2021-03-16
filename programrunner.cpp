#include "programrunner.h"
#include <QInputDialog>
#include <QtDebug>


ProgramRunner::ProgramRunner() {
    programContainer = new QMap<int, Statement*>();
}

ProgramRunner::~ProgramRunner() {
    programContainer->~QMap();
}

void ProgramRunner::setDisplay(QTextBrowser *cd, QTextBrowser *rd, QTextBrowser *sd, QLabel *ed) {
    code_display = cd;
    res_display = rd;
    syntax_display = sd;
    error_display = ed;
}

void ProgramRunner::execute_directly(Statement* stmt) {
    StatementType cur_t = stmt->getType();
    if (cur_t == LetStmt || cur_t == PrintStmt || cur_t == InputStmt) {
        stmt->execute(programContext);
    } else {
        error("Missing Line Number for this Statement.");
    }
}

void ProgramRunner::clear() {
    programContainer->clear();
    programContext.clear();
}

void ProgramRunner::readStatement(QVector<Token> tokens) {
    try {
    if (tokens[0].getType() == String) { //execute directly
        StringType cur_t = tokens[0].getWordType();
        tokens.pop_front();
        if (cur_t == LET) {
            LetStatement* exeStmt = new LetStatement(tokens);
            exeStmt->execute(programContext);
            code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == PRINT) {
            PrintStatement* exeStmt = new PrintStatement(tokens, res_display);
            exeStmt->execute(programContext);
            code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == INPUT) {
            InputStatement* exeStmt = new InputStatement(tokens);
            exeStmt->execute(programContext);
            code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == RUN) {
            code_display->insertPlainText("RUN\n");
            run_codes();
        } else if (cur_t == CLEAR) {
            clear();
            sync_display();
            code_display->insertPlainText("CLEAR\n");
        } else {
            error("Missing Line Number for this Statement.");
        }
        return;
    } else if (tokens[0].getType() == Number) { //normal statement
        int lineNumber = tokens[0].getNumber();
        tokens.pop_front();
        if (tokens.empty()) {
            programContainer->remove(lineNumber);
        } else {
            Statement *curStmt = parse(tokens);
            (*programContainer)[lineNumber] = curStmt;
            StatementTree *curTree = curStmt->getTree();
            syntax_display->insertPlainText(curTree->getSyntaxStr(lineNumber));
        }
        sync_display();
    }
    } catch (std::string msg) {
        error_display->setText("ERROR: " + QString::fromStdString(msg));
        return;
    }
}

void ProgramRunner::run_codes() {
    QMap<int, Statement*>::Iterator cur_code = programContainer->begin();
    while (cur_code != programContainer->end()) {
        int status = cur_code.value()->execute(programContext);
        switch (status) {
            case -2: return;
            case -1: error("Program Ends."); return;
            case 0:  cur_code++;break;
            default: {
                cur_code = programContainer->find(status);
                if (cur_code == programContainer->end()) {
                    error("LineNumber doesn't exist.");
                    return;
                }
            }
        }
    }
}

Statement *ProgramRunner::parse(QVector<Token> tokens) {
    StringType curWordType = tokens[0].getWordType();
    tokens.pop_front();
    Statement *curStmt;
    switch (curWordType) {
        case REM: {
            curStmt = new RemStatement(tokens); break;
        }
        case LET: {
            curStmt = new LetStatement(tokens); break;
        }
        case PRINT: {
            curStmt = new PrintStatement(tokens, res_display); break;
        }
        case INPUT: {
            curStmt = new InputStatement(tokens); break;
        }
        case GOTO: {
            curStmt = new GotoStatement(tokens); break;
        }
        case IF: {
            curStmt = new IfStatement(tokens); break;
        }
        case END: {
            curStmt = new EndStatement(); break;
        }
        default: curStmt = nullptr;
    }
    return curStmt;
}

void ProgramRunner::sync_display() {
    code_display->clear();
    QMap<int, Statement*>::Iterator cur_code = programContainer->begin();
    while (cur_code != programContainer->end()) {
        code_display->insertPlainText(QString::number(cur_code.key()) + " " +
                                      cur_code.value()->toString() + "\n");
        cur_code++;
    }
}
