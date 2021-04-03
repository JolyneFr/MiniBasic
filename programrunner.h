#ifndef PARSER_H
#define PARSER_H

#include "statement.h"
#include <QMap>

class ProgramRunner {
private:
    QMap<int, Statement*>* programContainer;
    EvaluationContext programContext;
    void execute_directly(Statement*);
    QTextBrowser* code_display;
    QTextBrowser* res_display;
    QTextBrowser* syntax_display;
    QLabel* error_display;

    Statement *parse(QVector<Token> tokens);
    void renew_error();
    void sync_display();

    void run_codes();
public:
    ProgramRunner();
    ~ProgramRunner();
    void clear();
    void setDisplay(QTextBrowser*, QTextBrowser*, QTextBrowser*, QLabel*);
    void readStatement(QVector<Token> tokens);
    void saveCode(std::string filename);
};


#endif // PARSER_H
