#include <iostream>
#include <unistd.h>

int main()
{

    while (true)
    {
        std::cout << "hello kjy , pid :" << getpid() << std::endl;
        sleep(1);
    }
    return 0;
}