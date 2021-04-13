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

QMap<QString, int> EvaluationContext::getTable() const { return symbolTable; }

void EvaluationContext::merge(EvaluationContext &global) {
    QMap<QString, int>::const_iterator cdata = global.getTable().cbegin();
    while (cdata != global.getTable().cend()) {
        symbolTable[cdata.key()] = cdata.value();
        cdata++;
    }
}
