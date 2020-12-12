//
// Created by jyf on 2020/12/2.
//

#include "codesim.h"

int Codesim::hash_k_gram(const vector<int> &k_gram) {
    int sum = 0;
    for (int i = (int) k_gram.size() - 1; i >= 0; i--) {
        sum *= 128;
        sum += k_gram[i];
    }
    string str;
    while (sum > 0) {
        str += (char) (sum % 128);
        sum = sum / 128;
    }
    return hash_string(str);
}

ostream &operator<<(ostream &stream, const CXString &str) {
    stream << clang_getCString(str);
    clang_disposeString(str);
    return stream;
}

CXTranslationUnit Codesim::clang_read(char *file_name) {
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
            index,
            file_name, nullptr, 0,
            nullptr, 0,
            CXTranslationUnit_None);

    clang_disposeIndex(index);
    return unit;
}

void Codesim::preprocess() {
    /*
     * preprocess two cpp files
     * delete all "include" in them to decrease the clang output
     * generate intermediate files
     */

    // read files and delete includes
    char *include = "#include";

    vector<string> code1, code2;
    string input;
    ifstream in_file;
    in_file.open(this->_file1, ios::in);
    if (!in_file.is_open()) {
        // report error
        cerr << "Error code 1: Unable to open file " << this->_file1 << ". Quitting." << endl;
        exit(-1);
    }
    while (getline(in_file, input)) {
        if (!strstr(input.c_str(), include) && (!input.empty())) {
            code1.push_back(input);
        }
    }
    in_file.close();

    in_file.open(this->_file2, ios::in);
    if (!in_file.is_open()) {
        // report error
        cerr << "Error code 1: Unable to open file " << this->_file2 << ". Quitting." << endl;
        exit(-1);
    }
    while (getline(in_file, input)) {
        if (!strstr(input.c_str(), include) && (!input.empty())) {
            code2.push_back(input);
        }
    }
    in_file.close();

    // generate intermediate files
    ofstream out_file;
    out_file.open(this->_temp1, ios::out);
    for (const string &str:code1) {
        out_file << str << endl;
    }
    out_file.close();

    out_file.open(this->_temp2, ios::out);
    for (const string &str:code2) {
        out_file << str << endl;
    }
    out_file.close();
}

void Codesim::clang_process() {
    /*
     * process temp files to generate AST
     * change AST to char* for later moss
     */

    // read temp files to generate AST
    this->_unit1 = this->clang_read(this->_temp1);
    this->_unit2 = this->clang_read(this->_temp2);

    // delete temp files
    remove(this->_temp1);
    remove(this->_temp2);

    if ((this->_unit1 == nullptr) || (this->_unit2 == nullptr)) {
        cerr << "Error code 2: Unable to parse translation unit. Quitting." << endl;
        exit(-1);
    }

    // change to hash
    CXCursor cursor_1 = clang_getTranslationUnitCursor(this->_unit1);
    clang_visitChildren(
            cursor_1,
            [](CXCursor c, CXCursor parent, CXClientData client_data) {
                _buf1.push_back(clang_getCursorKind(c));
//                cout << "Cursor '" << clang_getCursorSpelling(c) << "' of kind '"
//                     << clang_getCursorKind(c) << " " << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "'\n";
                return CXChildVisit_Recurse;
            },
            nullptr
    );

//    cout << endl;

    CXCursor cursor_2 = clang_getTranslationUnitCursor(this->_unit2);
    clang_visitChildren(
            cursor_2,
            [](CXCursor c, CXCursor parent, CXClientData client_data) {
                _buf2.push_back(clang_getCursorKind(c));
//                cout << "Cursor '" << clang_getCursorSpelling(c) << "' of kind '"
//                     << clang_getCursorKind(c) << " " << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "'\n";
                return CXChildVisit_Recurse;
            },
            nullptr
    );

    clang_disposeTranslationUnit(this->_unit1);
    clang_disposeTranslationUnit(this->_unit2);

//    for (int &i : _buf1) {
//        cout << i << " ";
//    }
//    cout << endl;
//    for (int &i : _buf2) {
//        cout << i << " ";
//    }
//    cout << endl;
}

vector<int> Codesim::k_gram(int k, vector<int> base) {
    vector<int> result;
    vector<int> k_gram;
    for (int i = 0; i < (int) base.size() - k + 1; i++) {
        k_gram.clear();
        for (int j = i; j < i + k; j++) {
            k_gram.push_back(base[j]);
        }
//        cout << "[";
//        for (int j : k_gram) {
//            cout << j << " ";
//        }
//        cout << "], ";
        result.push_back(this->hash_k_gram(k_gram));
    }
//    cout << endl;
    return result;
}

vector<hash_pair> hash_pairs(const vector<int> &grams, int w) {
    vector<hash_pair> result;
    int *sign = new int[(int) grams.size()];
    for (int i = 0; i < (int) grams.size(); i++) {
        sign[i] = 0;
    }
    for (int i = 0; i < (int) grams.size() - w + 1; i++) {
        int min_index = i, min_value = grams[i];
        for (int j = i + 1; j < i + w; j++) {
            if (grams[j] <= min_value) {
                min_index = j, min_value = grams[j];
            }
        }
        if (sign[min_index] == 0) {
            sign[min_index] = 1;
            hash_pair pair(min_value, min_index);
            result.push_back(pair);
        }
    }
    return result;
}

double Codesim::moss(int t, int k) {
    /*
     * _buf1 and _buf2 are ASTs
     * use moss to codesim them
     */

    // divide k-gram and compute hash
    vector<int> sample1 = (_buf1.size() < _buf2.size()) ? _buf1 : _buf2,
            sample2 = (_buf1.size() >= _buf2.size()) ? _buf1 : _buf2;
    vector<int> hash_k_gram_1 = this->k_gram(k, sample1), hash_k_gram_2 = this->k_gram(k, sample2);

    // divide into windows and compute hash pairs
    int w = t - k + 1;
    vector<hash_pair> pairs1 = hash_pairs(hash_k_gram_1, w), pairs2 = hash_pairs(hash_k_gram_2, w);

//    for (int i:hash_k_gram_1) {
//        cout << i << " ";
//    }
//    cout << endl;
//    for (hash_pair p: pairs1) {
//        cout << p.print() << " ";
//    }
//    cout << endl;
//    for (int i:hash_k_gram_2) {
//        cout << i << " ";
//    }
//    cout << endl;
//    for (hash_pair p: pairs2) {
//        cout << p.print() << " ";
//    }
//    cout << endl;

    // seek for length_t (that is, length_w of k_gram)
    int length = (int) hash_k_gram_1.size();
    int *sign = new int[length];
    for (int i = 0; i < length; i++)
        sign[i] = 0;
    for (hash_pair p1: pairs1) {
        for (hash_pair p2: pairs2) {
            if (p1._hash_num == p2._hash_num) {
                // find the longest match
                int match_start = 0, match_end = 1;
                while ((p1._location + match_start - 1 >= 0)
                       && (p2._location + match_start - 1 >= 0)
                       && (hash_k_gram_1[p1._location + match_start - 1] ==
                           hash_k_gram_2[p2._location + match_start - 1])) {
                    match_start--;
                }
                while ((p1._location + match_end < (int) hash_k_gram_1.size())
                       && (p2._location + match_end < (int) hash_k_gram_2.size())
                       &&
                       (hash_k_gram_1[p1._location + match_end] == hash_k_gram_2[p2._location + match_end])) {
                    match_end++;
                }

                if (match_end - match_start >= w) {
                    for (int i = match_start; i < match_end; i++) {
                        sign[p1._location + i] = 1;
                    }
                }
            }
        }
    }
    double sum = 0;
    for (int i = 0; i < length; i++) {
        sum += (double) sign[i];
    }
    return sum / (double) length * 100;
}

void Codesim::run() {
    this->init();
    this->preprocess();
    this->clang_process();
    cout << this->moss(this->_t, this->_k) << '%' << endl;
}