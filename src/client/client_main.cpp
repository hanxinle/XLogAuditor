#include <iostream>
#include "XClient.h"
using namespace std;

int main(int argc, char *argv[]) {
    string ip = "127.0.0.1";
    if (argc > 1) {
        ip = argv[0];
    }
    std::cout << "Client start!\n";
    if (!XClient::Get()->Init(ip)) {
        return 0;
    }
    XClient::Get()->Main();
    return 0;
}