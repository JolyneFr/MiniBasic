#ifndef PROGRAM_H
#define PROGRAM_H

#include "buffer.h"

using std::string;

class Program
{
private:
    Buffer *data_buffer;
public:
    Program();
    ~Program();
    void append(const string &text);
    void load(const string &filename);
    void save(const string &filename);
    void remove(int start, int end);
    void clear();
    int length();
};

#endif // PROGRAM_H
