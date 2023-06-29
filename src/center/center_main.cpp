#include "XCenter.h"
#include <iostream>
#include <string>

using namespace std;

bool Usage() {
    cout << "=========Center Usage=========" << endl;
    cout << "./center install 127.0.0.1" << endl;
    cout << "./center add 127.0.0.1 fileserver1" << endl;
    return true;
}


int main(int argc, char *argv[]) {
    string cmd;
    if (argc > 1) {
        cmd = argv[1];
    }
    if (cmd == "install") {
        cout << "Begin install center" << endl;
        if (argc < 3) {
            Usage();
            /*cout << "error:argc < 2 " << endl;*/
            return -1;
        }
        XCenter::Get()->Install(argv[2]);
    }

    if (cmd == "add") {
        cout << "add device" << endl;
        //./center add 127.0.0.1 fileserver1
        if (argc < 4) {
            Usage();
            return 0;
        }
        XCenter::Get()->AddDevice(argv[2], argv[3]);
        return 0;
    }

    cout << "Center start" << endl;
    XCenter::Get()->Main();
    return 0;
}