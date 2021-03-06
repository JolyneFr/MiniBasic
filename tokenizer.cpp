#include "tokenizer.h"

Tokenizer::Tokenizer(std::string str): cur_str(str) {}

std::vector<std::string> Tokenizer::getTokens() {
    std::vector<std::string> tokens;
    std::stringstream check(cur_str);
    std::string cur_token;
    while(getline(check, cur_token, ' ')) {
            tokens.push_back(cur_token);
    }
    return tokens;
}
