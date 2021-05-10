#include "programrunner.h"
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>
#include <QtDebug>


ProgramRunner::ProgramRunner() {
    programContainer = new QMap<int, Statement*>();
    programBuffer = new QMap<int, QString>();
    mode = Normal;
    debug_itr = programContainer->end();
}

ProgramRunner::~ProgramRunner() {
    programContainer->~QMap();
}

void ProgramRunner::setDisplay(QTextBrowser *cd, QTextBrowser *rd, QTextBrowser *sd, QLabel *ed, QTextBrowser *gd) {
    code_display = cd;
    res_display = rd;
    syntax_display = sd;
    error_display = ed;
    global_display = gd;
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
    programBuffer->clear();
    programContext.clear();
    renew_error();
    syntax_display->clear();
    res_display->clear();
    sync_display();
}

void ProgramRunner::start_debug() {
    mode = Debug;

    syntax_display->clear();
    res_display->clear();
    parse_codes();


    debug_itr = programContainer->begin();

    if (debug_itr == programContainer->end()) {
        end_debug(true);
        sync_display();
    } else {
        show_single_syntax();
    }

}

void ProgramRunner::show_single_syntax() {
    int cur_key = debug_itr.key();
    Statement* cur_stmt = debug_itr.value();
    StatementTree* cur_tree = cur_stmt->getTree();
    QString syntax_str = cur_tree->getSyntaxStr(cur_key);

    syntax_display->clear();
    syntax_display->insertPlainText(syntax_str);
    sync_display();
}

void ProgramRunner::step_next() {

    try {
        int status = debug_itr.value()->execute(programContext);
        sync_display();
        switch (status) {
            case -2: {
                end_debug(true);
                sync_display();
            }
            case -1: return; // parse error. keep the error message
            case 0:  debug_itr++ ;break;
            default: {
                debug_itr = programContainer->find(status);
                if (debug_itr == programContainer->end()) {
                    error("Target LineNumber doesn't exist.");
                    return;
                }
            }
        }
    }  catch (std::string msg) {
        error_display->setText("RUNTIME ERROR: " + QString::fromStdString(msg));
        end_debug(false);
        sync_display();
        return;
    }

    if (debug_itr == programContainer->end()) {
        end_debug(true);
        sync_display();
    } else {
        show_single_syntax();
    }

}

void ProgramRunner::end_debug(bool is_success) {
    mode = Normal;
    debug_itr = programContainer->end();
    if (is_success) {
        QMessageBox::information(NULL, "Debug Ends", "Program Ends Successfully!",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    } else {
        QMessageBox::information(NULL, "Debug Ends", "Program Terminated with Runtime Error",
                                 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
}

void ProgramRunner::step_end() {
    while (mode == Debug) {
        step_next();
    }
}

RunningMode ProgramRunner::get_mode() {
    return mode;
}

void ProgramRunner::readStatement(QString line) {
    try{
    renew_error();
    line = line.trimmed();
    if (!line.size()) return;
    Token begin = getTokens(line.toStdString())[0];
    if (begin.getType() == Number) {
        line = line.mid(begin.toString().length()).trimmed();
        if (line.length() == 0) {
            programBuffer->erase(programBuffer->find(begin.getNumber()));
        } else {
            programBuffer->insert(begin.getNumber(), line);
        }
        sync_display();
    } else if (begin.getType() == String) {
        StringType cur_t = begin.getWordType();
        line = line.mid(begin.toString().length()).trimmed();
        QVector<Token> tokens = getTokens(line.toStdString());
        if (cur_t == LET) {
            LetStatement* exeStmt = new LetStatement(tokens);
            exeStmt->execute(programContext);
            sync_display();
            //code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == PRINT) {
            PrintStatement* exeStmt = new PrintStatement(tokens, res_display);
            exeStmt->execute(programContext);
            //code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == INPUT) {
            InputStatement* exeStmt = new InputStatement(tokens);
            exeStmt->execute(programContext);
            sync_display();
            //code_display->insertPlainText(exeStmt->toString() + "\n");
        } else if (cur_t == RUN) {
            //code_display->insertPlainText("RUN\n");
            parse_codes();
            run_codes();
        } else if (cur_t == LIST) {
            // do nothing
        } else {
            error("Missing Line Number for this Statement. (wrong statement type)");
        }
        return;
    } else error("Unexpected beginning token.");
    } catch (std::string msg) {
        error_display->setText("INPUT ERROR: " + QString::fromStdString(msg));
        return;
    }
}

void ProgramRunner::parseStatement(int lineNumber, QVector<Token> tokens) {
    if (lineNumber <= 0)
        error("LineNumber must be greater than 0.");
    if (tokens.empty()) {
        programContainer->remove(lineNumber);
    } else {
        Statement *curStmt = parse(tokens);
        if (!curStmt)
           error("Type of statement doesn't exist.");
        (*programContainer)[lineNumber] = curStmt;
    }
}

void ProgramRunner::parse_codes() {
    programContainer->clear();
    QMap<int, QString>::Iterator cur_code = programBuffer->begin();
    while (cur_code != programBuffer->end()) {
        try {
        QVector<Token> tokens = getTokens(cur_code.value().toStdString());
        parseStatement(cur_code.key(), tokens);
        cur_code++;
        } catch (std::string msg) {
            error_display->setText("PARSE ERROR: " + QString::fromStdString(msg) +
                                   "at line " + QString::number(cur_code.key()));
            (*programContainer)[cur_code.key()] = new ErrorStatement(QString::fromStdString(msg));
            sync_display();
            return;
        }
    }
}

void ProgramRunner::run_codes() {
    QMap<int, Statement*>::Iterator cur_code = programContainer->begin();
    try {
    syntax_display->clear();
    res_display->clear();
    // display the syntax tree first
    while (cur_code != programContainer->end()) {
        Statement* curStmt = cur_code.value();
        int lineNumber = cur_code.key();
        StatementTree *curTree = curStmt->getTree();
        QString syntaxStr = curTree->getSyntaxStr(lineNumber);
        syntax_display->insertPlainText(curTree->getSyntaxStr(lineNumber));
        cur_code++;
    }
    // then run the codes. What an idiot idea!
    cur_code = programContainer->begin();
    while (cur_code != programContainer->end()) {
        int status = cur_code.value()->execute(programContext);
        sync_display();
        switch (status) {
            case -2: return;
            case -1: return; // parse error. keep the error message
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
    switch (curWordType) {
        case REM: {
            return new RemStatement(tokens);
        }
        case LET: {
            return new LetStatement(tokens); break;
        }
        case PRINT: {
            return new PrintStatement(tokens, res_display); break;
        }
        case PRINTF: {
            return new PrintfStatement(tokens, res_display); break;
        }
        case INPUT: {
            return new InputStatement(tokens); break;
        }
        case INPUTS: {
            return new InputsStatement(tokens); break;
        }
        case GOTO: {
            return new GotoStatement(tokens); break;
        }
        case IF: {
            return new IfStatement(tokens); break;
        }
        case END: {
            return new EndStatement(); break;
        }
        default: return nullptr;
    }
}

void ProgramRunner::sync_display() {
    code_display->clear();
    global_display->clear();
    if (mode == Debug) {
        code_display->append("<p style=\"background:white;margin:0\">----------------Debug Mode----------------</p>");
    } else {
        code_display->append("<p style=\"background:white;margin:0\">----------------Normal Mode----------------</p>");
    }
    QMap<int, QString>::Iterator cur_code = programBuffer->begin();
    while (cur_code != programBuffer->end()) {
        QString text = QString::number(cur_code.key()) + " " + cur_code.value();
        if (programContainer->count(cur_code.key()) &&
                (*programContainer)[cur_code.key()]->getType() == ErrorStmt) {
            text = "<p style=\"background:red;color:#ffffff;margin:0\">" + text + "</p>";
        } else if (mode == Debug && programBuffer->count(debug_itr.key()) && cur_code.key() == debug_itr.key()) {
            text = "<p style=\"background:green;color:#ffffff;margin:0\">" + text + "</p>";
        } else{
            text = "<p style=\"background:white;color:#000000;margin:0\">" + text + "</p>";
        }
        code_display->append(text);
        cur_code++;
    }

    QMap<QString, QString>::ConstIterator cur_string = programContext.getStringTable().cbegin();
    while (cur_string != programContext.getStringTable().cend()) {
        global_display->insertPlainText(cur_string.key() + ": STR = " +
                                        cur_string.value() + "\n");
        cur_string++;
    }
    QMap<QString, int>::ConstIterator cur_variable = programContext.getTable().cbegin();
    while (cur_variable != programContext.getTable().cend()) {
        global_display->insertPlainText(cur_variable.key() + ": INT = " +
                                        QString::number(cur_variable.value()) + "\n");
        cur_variable++;
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
