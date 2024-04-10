#pragma once
#include <iostream>

class Client{
    public:
    Client(){
        std::cout << "Client created\n";
    };
    void Start();
};