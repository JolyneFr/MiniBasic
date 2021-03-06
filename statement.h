#ifndef STATEMENT_H
#define STATEMENT_H

/*
 * Type: StatementType
 * -------------------
 * This enumerated type is used to differntiate the seven different
 * statement types: RemStmt, LetStmt, PrintStmt, InputSymt, GotoStmt,
 * IfStmt, EndStmt.
 */

enum StatementType { RemStmt, LetStmt, PrintStmt, InputSymt,
                     GotoStmt, IfStmt, EndStmt };

/*
 * Class: Statement
 * ----------------
 * This class is used to encapsulate a single statement in program.
 * Every Statement object is created using one of the seven concrete
 * subclasses shown above.
 */
class Statement {

public:

    Statement();
    virtual ~Statement();
    virtual StatementType type() = 0;

};

#endif // STATEMENT_H
