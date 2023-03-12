#include "balancer.h"

#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        Balancer balancer("./../../balancer/file.txt"); //такой формат файла пока идет разработка, потом я в отдельную папку вынесу все исполняемые и там сделаю файл конфигурации
        balancer.connect();
        balancer.distribute();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}