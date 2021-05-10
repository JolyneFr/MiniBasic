#include "evalstate.h"

void EvaluationContext::setValue(QString var, int value) {
    symbolTable[var] = value;
}

void EvaluationContext::setValue(QString var, QString value) {
    stringTable[var] = value;
}

int EvaluationContext::getValue(QString var) {
    QMap<QString, int>::iterator find_itr = symbolTable.find(var);
    if (find_itr == symbolTable.end()) {
        throw (var + " is undefined").toStdString();
    }
    return find_itr.value();
}

QString EvaluationContext::getString(QString var) {
    auto find_itr = stringTable.find(var);
    if (find_itr == stringTable.end()) {
        throw (var + " is undefined").toStdString();
    }
    return find_itr.value();
}

bool EvaluationContext::isDefinedInt(QString var) {
    return symbolTable.count(var);
}

bool EvaluationContext::isDefinedString(QString var) {
    return stringTable.count(var);
}

void EvaluationContext::clear() {
    symbolTable.clear();
}

QMap<QString, int> EvaluationContext::getTable() const { return symbolTable; }

QMap<QString, QString> EvaluationContext::getStringTable() const { return stringTable; }

void EvaluationContext::merge(EvaluationContext &global) {
    QMap<QString, int>::const_iterator cdata = global.getTable().cbegin();
    while (cdata != global.getTable().cend()) {
        symbolTable[cdata.key()] = cdata.value();
        cdata++;
    }
}
