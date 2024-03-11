#include <iostream>
#include <cstdio>

#include "Config.h"
#include "net_test.h"
#include "Server.h"
#include "Client.h"

int main(int argc, char* argv[]){

    std::cout << "Welcome to CPP_TEMPLATE version " << CPP_TEMPLATE_VERSION_MAJOR << "." << CPP_TEMPLATE_VERSION_MINOR << "\n";

//Netowrking test 
    Test();

    Server();
    
    Client();

    std::getchar();

    return 0;
}
