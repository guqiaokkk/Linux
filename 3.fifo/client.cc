#include "namedPipe.hpp"

// write
int main()
{
    NamePiped fifo(comm_path, User);
    if (fifo.OpenForWrite())
    {
        while (true)
        {

            std::cout << "please enter > ";
            std::string message;
            std::getline(std::cin, message);
            fifo.WriteNamePipe(message);
        }
    }
    return 0;
}