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
   int getValue(QString var);
   bool isDefined(QString var);
   void clear();

private:

   QMap<QString, int> symbolTable;

};

#endif // EVALSTATE_H
