#ifndef PARSER_H
#define PARSER_H

#include "statement.h"
#include <QMap>

class ProgramRunner {
private:
    QMap<int, Statement*>* programContainer;
    QMap<int, QString>* programBuffer;
    EvaluationContext programContext;
    void execute_directly(Statement*);
    QTextBrowser* code_display;
    QTextBrowser* res_display;
    QTextBrowser* syntax_display;
    QTextBrowser* global_display;
    QLabel* error_display;

    Statement *parse(QVector<Token> tokens);
    void parseStatement(int lineNumber, QVector<Token> tokens);
    void renew_error();
    void sync_display();

public:
    ProgramRunner();
    ~ProgramRunner();
    void clear();
    void run_codes();
    void parse_codes();
    void setDisplay(QTextBrowser*, QTextBrowser*, QTextBrowser*, QLabel*, QTextBrowser*);
    void readStatement(QString);
    void saveCode(std::string filename);
};


#endif // PARSER_H
