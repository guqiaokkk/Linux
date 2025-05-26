#include "shm.hpp"
#include "namedPipe.hpp"

#include <csignal>
#include <memory>
#include <bits/unique_ptr.h>

std::unique_ptr<Shm> global_shm;  // 使用智能指针自动管理

void signal_handler(int signum) {
    std::cout << "Received signal " << signum << ", cleaning up..." << std::endl;
    global_shm.reset();  // 显式释放 Shm 对象
    exit(signum);
}

int main()
{
    // 注册信号处理函数
    signal(SIGINT, signal_handler);  // Ctrl+C,他表示收到SIGINT信号后，调用sianal_handler
    signal(SIGTERM, signal_handler); // kill 命令


    //1.创建共享内存
    global_shm = std::make_unique<Shm>(gpathname, gproj_id, gCreater);
    char* shmaddr = (char*)global_shm->Addr();
    global_shm->Zero();


    //2.创建管道
    NamePiped fifo(comm_path,Creater);
    fifo.OpenForRead();


    while (true)
    {
        std::string tmp;
        fifo.ReadNamePipe(&tmp);//没读到就会在这等待

        if (tmp == "exit") break;  // 安全退出条件

        std::cout << "shm memory content : " << shmaddr << std::endl;
    }
    

    return 0;
}