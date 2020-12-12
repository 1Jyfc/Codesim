#include <iostream>

#include "codesim.h"

int main(int argc, char *argv[]) {
    if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0)) {
        cout << "usage: ./codesim [-h|--help] code1 code2" << endl;
    } else {
        Codesim codesim(argv[1], argv[2]);
        codesim.run();
    }
    return 0;
}
