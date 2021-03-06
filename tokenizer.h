#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>
#include <string>
#include <sstream>

class Tokenizer {

public:

    Tokenizer(std::string str);
    std::vector<std::string> getTokens();

private:

    std::string cur_str;

};

#endif // TOKENIZER_H
