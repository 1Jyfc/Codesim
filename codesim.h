//
// Created by jyf on 2020/12/2.
//

#ifndef CODESIM_CODESIM_H
#define CODESIM_CODESIM_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <functional>
#include <clang-c/Index.h>

using namespace std;

static vector<int> _buf1, _buf2;

struct hash_pair {
    int _hash_num;
    int _location;

    hash_pair(int hash_num, int location) {
        this->_hash_num = hash_num;
        this->_location = location;
    }

    string print() const {
        return "[" + to_string(this->_hash_num) + ", " + to_string(this->_location) + "]";
    }
};

class Codesim {
public:
    Codesim(char *file_1, char *file_2) {
        this->_file1 = new char[strlen(file_1) + 3];
        if ((file_1[0] != '~') && (file_1[0] != '/')) {
            this->_file1[0] = '.', this->_file1[1] = '/';
            strncpy(this->_file1 + 2, file_1, strlen(file_1));
        } else {
            strncpy(this->_file1, file_1, strlen(file_1));
        }

        this->_file2 = new char[strlen(file_2) + 3];
        if ((file_2[0] != '~') && (file_2[0] != '/')) {
            this->_file2[0] = '.', this->_file2[1] = '/';
            strncpy(this->_file2 + 2, file_2, strlen(file_2));
        } else {
            strncpy(this->_file2, file_2, strlen(file_2));
        }
    }

    ~Codesim() {}

    void run();

private:
    char *_temp1, *_temp2;
    char *_file1, *_file2;
    CXTranslationUnit _unit1, _unit2;
    int _t, _k;

    hash<string> hash_string;

    static CXTranslationUnit clang_read(char *file_name);

    void init() {
        _temp1 = "temp1.cpp";
        _temp2 = "temp2.cpp";
        _buf1.clear();
        _buf2.clear();
        this->_t = 4;
        this->_k = 2;
    }

    int hash_k_gram(const vector<int> &k_gram);

    vector<int> k_gram(int k, vector<int> base);

    void preprocess();

    void clang_process();

    double moss(int t, int k);
};

#endif //CODESIM_CODESIM_H
