#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <QVector>
#include <string>
#include <QString>

enum TokenType { Number, String, Mark };
enum StringType { REM, LET, PRINT, PRINTF, INPUT, INPUTS, GOTO, IF, END, THEN,
                RUN, LOAD, LIST, CLEAR, HELP, QUIT, Variable, NONE };

class Token {
private:
    QString tokenString;
    void resolve();
    TokenType tType;
    StringType sType;
public:
    Token(QString ts, TokenType tt);
    TokenType getType();
    StringType getWordType();
    int getNumber();
    QString toString();
    QString getOp();
};

QVector<Token> getTokens(std::string str);

QVector<Token> parse_format(QString fmt);

#endif // TOKENIZER_H
