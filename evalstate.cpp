#include "evalstate.h"

void EvaluationContext::setValue(std::string var, int value) {
    symbolTable[var] = value;
}

int EvaluationContext::getValue(std::string var) {
    std::map<std::string, int>::iterator find_itr = symbolTable.find(var);
    if (find_itr == symbolTable.end()) {
        throw (var + " is undefined");
    }
    return find_itr->second;
}

bool EvaluationContext::isDefined(std::string var) {
    return symbolTable.count(var);
}
