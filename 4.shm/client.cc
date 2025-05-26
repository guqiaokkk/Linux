#include "shm.hpp"
#include "namedPipe.hpp"


int main()
{
    //1.创建共享内存
    Shm shm(gpathname,gproj_id,gUser);
    shm.Zero();
    char* shmaddr = (char*)shm.Addr();
    
    //2.创建管道
       NamePiped fifo(comm_path,User);
      fifo.OpenForWrite();
  
    //当成字符串 
    char ch = 'A';
    while(ch <= 'Z')
    {
        shmaddr[ch - 'A'] = ch;

        std::string tmp = "wakeup";
        std::cout << "add" << ch << "into shm," << "wakeup reader" << std::endl;
        fifo.WriteNamePipe(tmp);
        sleep(2);
        ch++;
    }

    return 0;
} 