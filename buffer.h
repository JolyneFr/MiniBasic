#ifndef BUFFER_H
#define BUFFER_H

#include <string>

using std::string;

class Buffer {
private:
    struct node {
        string data;
        node* next = nullptr;
        node* pre = nullptr;
    };
    class List {
    private:
        node* head;
        node* rear;
    public:
        int current_address;
        int current_length;
        List();
        ~List();
        node* access(int idx);
        void insert_line_after(string curdata, int idx);
        void delete_line(int idx);
        void print_line(int idx);
    };
public:
    List* data_list;
    Buffer();
    ~Buffer();

    void writeToFile(const string &filename) const;

    const string &moveToLine(int idx) const;

    void showLines(int from, int to) const;

    void deleteLines(int from, int to);
    void insertLine(const string &text);
    void appendLine(const string &text);
};

#endif // BUFFER_H
