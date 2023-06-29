#include <iostream>
#include "XAgent.h"

using namespace std;
int main (int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "please set ip" << endl;

    }
    if (!XAgent::Get ()->Init (argv[1])) {
        cout << "Init mysql failure." << endl;
        return -1;
    }
    cout << "Init mysql success." << endl;
    XAgent::Get ()->Main ();
    return 0;
}