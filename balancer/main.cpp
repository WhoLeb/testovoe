#include "balancer.h"

#include <iostream>
#include <stdexcept>
#include <sstream>

int main(int argc, char *argv[])
{
    try
    {
        std::stringstream filepath;
        filepath << argv[1];
        Balancer balancer(filepath.str());
        balancer.connect();
        balancer.distribute();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}