#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <QString>
#include <QMap>

/*
 * Class: EvaluationContext
 * ------------------------
 * This class encapsulates the information that the evaluator needs to
 * know in order to evaluate an expression.
 */

class EvaluationContext {

public:

   void setValue(QString var, int value);
   void setValue(QString var, QString value);
   int getValue(QString var);
   QString getString(QString var);
   bool isDefinedInt(QString var);
   bool isDefinedString(QString var);
   void clear();
   void merge(EvaluationContext &);
   QMap<QString, int> getTable() const;
   QMap<QString, QString> getStringTable() const;

private:

   QMap<QString, int> symbolTable;
   QMap<QString, QString> stringTable;

};

#endif // EVALSTATE_H
