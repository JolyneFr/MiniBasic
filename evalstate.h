#ifndef EVALSTATE_H
#define EVALSTATE_H

#include <string>
#include <map>

/*
 * Class: EvaluationContext
 * ------------------------
 * This class encapsulates the information that the evaluator needs to
 * know in order to evaluate an expression.
 */

class EvaluationContext {

public:

   void setValue(std::string var, int value);
   int getValue(std::string var);
   bool isDefined(std::string var);

private:

   std::map<std::string, int> symbolTable;

};

#endif // EVALSTATE_H
