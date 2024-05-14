#include <iostream>
#include <memory>
#include "Kosbot.h"

int main(int argc, char** argv){
    
    std::shared_ptr<Kosbot> kosbot = std::make_shared<Kosbot>();
    kosbot->Run();

    return 1;
}