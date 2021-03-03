#include "program.h"

Program::Program()
{
    data_buffer = new Buffer();
}

void Program::append(const string &text)
{
    data_buffer->appendLine(text);
}

void Program::clear()
{
    data_buffer->~Buffer();
    data_buffer = new Buffer();
}
