#include "programrunner.h"
#include <QInputDialog>
#include <fstream>
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

void ProgramRunner::renew_error() {
    error_display->setText("No error detected.");
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
    renew_error();
    if (tokens[0].getType() == String) { // execute directly
        StringType cur_t = tokens[0].getWordType();
        tokens.pop_front();
        if (cur_t == LET) {
            LetStatement* exeStmt = new LetStatement(tokens);
            exeStmt->execute(programContext);
            //code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == PRINT) {
            PrintStatement* exeStmt = new PrintStatement(tokens, res_display);
            exeStmt->execute(programContext);
            //code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == INPUT) {
            InputStatement* exeStmt = new InputStatement(tokens);
            exeStmt->execute(programContext);
            //code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == RUN) {
            //code_display->insertPlainText("RUN\n");
            run_codes();
        } else if (cur_t == LIST) {
            // do nothing
        } else {
            error("Missing Line Number for this Statement. (wrong statement type)");
        }
        return;
    } else if (tokens[0].getType() == Number) { //normal statement
        int lineNumber = tokens[0].getNumber();
        if (lineNumber <= 0)
            error("LineNumber must be greater than 0.");
        tokens.pop_front();
        if (tokens.empty()) {
            programContainer->remove(lineNumber);
        } else {
            Statement *curStmt = parse(tokens);
            if (!curStmt)
                error("Type of statement doesn't exist.");
            (*programContainer)[lineNumber] = curStmt;
        }
        sync_display();
    } else {
        error("Unexpected beginning: Should be LineNumber or Statement.");
    }
    } catch (std::string msg) {
        error_display->setText("ERROR: " + QString::fromStdString(msg));
        return;
    }
}

void ProgramRunner::run_codes() {
    QMap<int, Statement*>::Iterator cur_code = programContainer->begin();
    try {
    while (cur_code != programContainer->end()) {
        Statement* curStmt = cur_code.value();
        int lineNumber = cur_code.key();
        StatementTree *curTree = curStmt->getTree();
        syntax_display->insertPlainText(curTree->getSyntaxStr(lineNumber));
        int status = cur_code.value()->execute(programContext);
        switch (status) {
            case -2: return;
            case -1: error("Unexpected program interruption."); return;
            case 0:  cur_code++;break;
            default: {
                cur_code = programContainer->find(status);
                if (cur_code == programContainer->end()) {
                    error("Target LineNumber doesn't exist.");
                    return;
                }
            }
        }
    }
    } catch (std::string msg) {
        error_display->setText("RUNTIME ERROR: " + QString::fromStdString(msg));
        return;
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

void ProgramRunner::saveCode(std::string filename) {
    std::fstream this_file;
    this_file.open(filename, std::ios::out);
    QMap<int, Statement*>::Iterator cur_code = programContainer->begin();
    while (cur_code != programContainer->end()) {
        QString cur_text = QString::number(cur_code.key()) + " ";
        cur_text += cur_code.value()->toString();
        this_file << cur_text.toStdString() << std::endl;
        cur_code++;
    }
    this_file.close();
}
