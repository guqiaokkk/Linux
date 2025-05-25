#include "namedPipe.hpp"

// read 管理命名管道的整个生命周期
int main()
{
    NamePiped fifo(comm_path, Creater); // 一人做就好，另一个人只用打开
    //对于读端而言，如果我们打开文件，但是写还没来，我会阻塞在open调用中，直到对方打开
    //进程同步
    if (fifo.OpenForRead()) // read身份
    {
        while (true)
        {
            std::string message;
            int n = fifo.ReadNamePipe(&message);
            if (n > 0)
            {
                std::cout << "client say : " << message << std::endl;
            }
            else if(n == 0)
            {
                std::cout << "Client quit,Server too" << std::endl;
                break;
            }
            else
            {
                std::cout << "fifo.ReadNamePipe error" << std::endl;
                break;
            }
        }
    }

    return 0;
}