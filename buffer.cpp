#include <fstream>
#include <iostream>
#include "buffer.h"

//TODO: your code here
//implement the functions in ListBuffer

Buffer::List::List() {
    head = new node();
    rear = new node();
    head -> next = rear;
    rear -> pre = head;
    current_address = 0;
    current_length = 0;
}

Buffer::List::~List() {
    node* p = head -> next;
    node* q;
    while (p != rear) {
        q = p -> next;
        delete p;
        p = q;
    }
    delete head;
    delete rear;
}

Buffer::node* Buffer::List::access(int idx) {
    int counter = idx;
    node* cur_node = head;
    while (counter) {
        cur_node = cur_node -> next;
        counter--;
    }
    return cur_node;
}

void Buffer::List::delete_line(int idx) {
    node* cur_node = access(idx);
    node* pre_node = cur_node -> pre;
    node* next_node = cur_node -> next;
    pre_node -> next = next_node;
    next_node -> pre = pre_node;
    delete cur_node;
    current_length--;
}

void Buffer::List::insert_line_after(string curdata, int idx) {
    node* pre_node = access(idx);
    node* next_node = pre_node -> next;
    node* cur_node = new node();
    cur_node -> data = curdata;
    pre_node -> next = cur_node;
    next_node -> pre = cur_node;
    cur_node -> pre = pre_node;
    cur_node -> next = next_node;
    current_length++;
}

void Buffer::List::print_line(int idx) {
    node* cur_node = access(idx);
    std::cout << cur_node -> data << std::endl;
}

Buffer::Buffer() {
    data_list = new List();
}

Buffer::~Buffer() {
    data_list->~List();
}

void Buffer::writeToFile(const string &filename) const {
    std::fstream this_file;
    this_file.open(filename, std::ios::out);
    int byte_counter = 0;
    int line_counter = 1;
    while (line_counter <= data_list -> current_length) {
        string cur_data = data_list -> access(line_counter) -> data;
        this_file << cur_data << std::endl;
        byte_counter += cur_data.length();
        line_counter++;
    }
    std::cout << byte_counter + data_list -> current_length << " byte(s) written" << std::endl;
    this_file.close();
}

void Buffer::readFromFile(const string &filename) { /* clear the buffer before call this function */
    std::fstream this_file;
    this_file.open(filename, std::ios::in);
    std::string text;
    while (std::getline(this_file, text)) {
        appendLine(text);
    }
    this_file.close();
}

void Buffer::showLines(int from, int to) const {
    for(int i = from; i <= to; i++) {
        std::cout << i << "\t";
        data_list -> print_line(i);
    }
}

void Buffer::deleteLines(int from, int to){
    int del_counter = to - from + 1;
    for(int i = 0; i < del_counter; i++) {
        data_list -> delete_line(from);
    }
    data_list -> current_address = data_list -> current_length;
}

void Buffer::insertLine(const string &text){
    if (data_list -> current_address == 0) {
        data_list -> insert_line_after(text, 0);
        data_list -> current_address++;
    } else {
        data_list -> insert_line_after(text, data_list -> current_address - 1);
        data_list -> current_address++;
    }
}

void Buffer::appendLine(const string &text){
    data_list -> insert_line_after(text, data_list -> current_address);
    data_list -> current_address++;
}

const string &Buffer::moveToLine(int idx) const {
    data_list -> current_address = idx;
    return data_list -> access(idx) -> data;
}
