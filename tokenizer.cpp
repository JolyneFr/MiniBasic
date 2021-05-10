#include "tokenizer.h"
#include <QtDebug>

static QString matchStr[16] = {"REM", "LET", "PRINT", "PRINTF", "INPUT", "INPUTS", "GOTO", "IF", "END", "THEN",
                       "RUN", "LOAD", "LIST", "CLEAR", "HELP", "QUIT"};

Token::Token(QString ts, TokenType tt): tokenString(ts), tType(tt) {
    resolve();
    //qDebug() << ts;
}

void Token::resolve() {
    if (tType == String) sType = Variable;
    else sType = NONE;
    for (int i = 0; i < 14; i++) {
        if (tokenString == matchStr[i]) {
            sType = StringType(i);
        }
    }
}

TokenType Token::getType() { return tType; }

StringType Token::getWordType() { return sType; }

int Token::getNumber() {
    if (tType == Number) {
        return tokenString.toInt();
    }
    return -1;
}

QString Token::toString() {
    return tokenString;
}

QString Token::getOp() {
    if (tType == Mark) {
        return tokenString;
    }
    return "";
}

QVector<Token> getTokens(std::string cur_str) {
    QString qcur_str = QString::fromStdString(cur_str);
    QVector<Token> tokens;
    QString cur_token;
    int index = 0;
    while (cur_str[index] != '\0') {
        //qDebug() << "First char: " << cur_str[index];
        if (cur_token.isEmpty()) {
            cur_token.append(qcur_str[index]);
            index++;

            if (cur_token[0] == '"') {
                tokens.append(Token(cur_token, Mark));
                cur_token.clear();

                while (cur_str[index] != '"' && cur_str[index] != '\0') {
                    cur_token.append(qcur_str[index]);
                    index++;
                }
                if (cur_str[index] == '\0') {
                    tokens.append(Token(cur_token, String));
                    break;
                }
                tokens.append(Token(cur_token, String));
                tokens.append(Token("\"", Mark));
                cur_token.clear();
                index++;
                continue;
            }

            if (cur_token[0] == '\'') {
                tokens.append(Token(cur_token, Mark));
                cur_token.clear();

                while (cur_str[index] != '\'' && cur_str[index] != '\0') {
                    cur_token.append(qcur_str[index]);
                    index++;
                }
                if (cur_str[index] == '\0') {
                    tokens.append(Token(cur_token, String));
                    break;
                }
                tokens.append(Token(cur_token, String));
                tokens.append(Token("'", Mark));
                cur_token.clear();
                index++;
                continue;
            }

            if (cur_token[0] == '<' || cur_token[0] == '>' || cur_token[0] == '=' || cur_token[0] == '(' ||
                cur_token[0] == ')' || cur_token[0] == '+' || cur_token[0] == ',' ||
                    (cur_token[0] == '-' && (index > 0 && (tokens.back().getType() != Mark && ((tokens.back().getType() == String) == (tokens.back().getWordType() == Variable))))) ||
                    cur_token[0] == '/' ) {
                tokens.append(Token(cur_token, Mark));
                cur_token.clear();
                continue;
            }
            if (cur_token[0] == '*') {
                if (cur_str[index] == '*') {
                    cur_token.append(qcur_str[index]);
                    index++;
                }
                tokens.append(Token(cur_token, Mark));
                cur_token.clear();
                continue;
            }
        }
        if (cur_token[0].isNumber() || cur_token[0] == '-') {
            while (qcur_str[index].isNumber()) {
                cur_token.append(qcur_str[index]);
                index++;
            }
            if (cur_token == "-") {
                tokens.append(Token(cur_token, Mark));
            } else {
                QVector<Token>::iterator last = tokens.end() - 1;
                QVector<Token>::iterator before_last = last - 1;
                if (tokens.length() && last->toString() == "-" && cur_token[0] != '-' &&
                        (before_last->toString() == "=" || before_last->toString() == "(")) {
                    tokens.pop_back();
                    cur_token.push_front('-');
                }
                tokens.append(Token(cur_token, Number));
            }
            cur_token.clear();
        }
        if (cur_token[0].isLetter()) {
            while (qcur_str[index].isLetterOrNumber()) {
                cur_token.append(qcur_str[index]);
                index++;
            }
            tokens.append(Token(cur_token, String));
            cur_token.clear();
        }
        while (qcur_str[index].isSpace()) {
            index++;
        }
        cur_token.clear();
    }
    return tokens;
}

QVector<Token> parse_format(QString fmt) {
    QVector<Token> tokens;
    QString cur_token;
    int index = 0;
    while (fmt[index] != '\0') {
        if (fmt[index] != '{' && fmt[index] != '}') {
            cur_token.append(fmt[index]);
            index++;
        } else if (fmt[index] == '{') {
            if (index >= fmt.size() - 1 || fmt[index + 1] != '}') {
                throw "Single '{' or '}' is not allowed.";
            }
            tokens.append(Token(cur_token, String));
            cur_token.clear();
            tokens.append(Token("{}", String));
            index += 2;
        } else throw "Single '{' or '}' is not allowed.";
    }
    if (cur_token.size() != 0) {
        tokens.append(Token(cur_token, String));
    }
    return tokens;
}
