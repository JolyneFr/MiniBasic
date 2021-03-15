#include "evalstate.h"

void EvaluationContext::setValue(QString var, int value) {
    symbolTable[var] = value;
}

int EvaluationContext::getValue(QString var) {
    QMap<QString, int>::iterator find_itr = symbolTable.find(var);
    if (find_itr == symbolTable.end()) {
        throw (var + " is undefined");
    }
    return find_itr.value();
}

bool EvaluationContext::isDefined(QString var) {
    return symbolTable.count(var);
}

void EvaluationContext::clear() {
    symbolTable.clear();
}
