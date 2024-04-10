#include <iostream>
#include "Core.h"
#include "Networking.h"
#include "test.h"
int main(int argc, char* argv[]){
    std::cout << "Hello World\n";

    Core* c = new Core();
    c->Init();
    delete c;

    Networking* n = new Networking();
    delete n;

    std::cout << test() << "\n";
    return 0;
}