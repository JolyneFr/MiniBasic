#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <string>

void error(std::string msg) {
    throw msg;
}

#endif // ERRORHANDLER_H
