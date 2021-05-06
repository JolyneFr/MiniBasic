#ifndef PARSER_H
#define PARSER_H

#include "statement.h"
#include <QMap>

enum RunningMode { Normal, Debug };

class ProgramRunner {
private:

    QMap<int, Statement*>* programContainer;
    QMap<int, QString>* programBuffer;
    EvaluationContext programContext;
    RunningMode mode;
    QMap<int, Statement*>::Iterator debug_itr;
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
    void end_debug(bool is_success);
    void show_single_syntax();

public:
    ProgramRunner();
    ~ProgramRunner();
    void clear();
    void run_codes();
    void parse_codes();
    void setDisplay(QTextBrowser*, QTextBrowser*, QTextBrowser*, QLabel*, QTextBrowser*);
    void readStatement(QString);
    void saveCode(std::string filename);

    void start_debug();
    void step_next();
    void step_end();
    RunningMode get_mode();
};


#endif // PARSER_H
