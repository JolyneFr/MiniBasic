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

void Program::save(const string &filename)
{
    data_buffer->writeToFile(filename);
}

void Program::load(const string &filename)
{
    clear();
    data_buffer->readFromFile(filename);
}

void Program::remove(int start, int end)
{
    data_buffer->deleteLines(start ,end);
}

int Program::length()
{
    return data_buffer->data_list->current_length;
}
