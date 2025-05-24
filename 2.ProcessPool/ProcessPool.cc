#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include "Task.hpp"
#include <sys/wait.h>

// master视角
class Channel
{
public:
    Channel(int wfd, pid_t subprocessid, std::string name) : _wfd(wfd), _subprocessid(subprocessid), _name(name)
    {
    }
    int Getwfd() { return _wfd; }
    pid_t GetProcess() { return _subprocessid; }
    std::string Getname() { return _name; }
    
    void ChannelClose()
    {
        close(_wfd);
    }
    
    void Wait()
    {
        pid_t rid = waitpid(_subprocessid,nullptr,0);
        if(rid > 0)
        {
            std::cout << "wait" << rid << "success" << std::endl;
        }
    }

    ~Channel()
    {
    }

private:
    int _wfd;            // 管道写端                    通过这个
    pid_t _subprocessid; // 子进程的pid      写给这个
    std::string _name;
};

// void work(int rfd)
// {
//     while (true)
//     {
//         int command = 0;
//         int n = read(rfd, &command, sizeof(command));
//         // read 从文件描述符 rfd 中读取数据，并将其存储到 command 的内存中。
//         if (n == sizeof(int))
//         {
//             ExcuteTask(command);
//         }
//         else if(n == 0)
//         {
//             std::cout << "sub process:" << getpid() << "quit" << std::endl;
//             break;
//         }
//     }
// }


// 形参类型和规范
// const &:输出型参数
//  & ： 输入输出型参数
//  * ： 输出型参数



// 1.创建信道和子进程
void CreateChannelAndSub(int num, std::vector<Channel> *channels,task_t task)
{
    
    for (int i = 0; i < num; i++)
    {
        // 1.创建管道
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        if (n < 0)
            exit(1);

        // 2.创建子进程
        pid_t id = fork();
        if (id == 0)
        {
            if(!channels->empty())//从第二次开始，创建的管道
            {
                for(auto& channel : *channels)channel.ChannelClose();//把所有位置的写端关了
            }
            // child - read
            close(pipefd[1]);
            dup2(pipefd[0],0);//将管道的读端，重定向到标准输入，此时work可以不用传参
            //work(pipefd[0]);
            task();
            close(pipefd[0]);
            exit(0);
        }

        // 3.构建channel名字
        std::string channe_name = "Chanel-" + std::to_string(i);

        // father - w
        close(pipefd[0]); // a 子进程的pid   b 父进程关心的管道的w端
        channels->push_back(Channel(pipefd[1], id, channe_name));
    }
}




// 例如channelnum = 3,他会一直在0，1，2，中返回
int NextChannel(int channelnum) // 信道的下标，用于选一个信道
{
    static int next = 0;
    int channel = next;
    next++;
    next %= channelnum;
    return channel;
}

// 发送
void SendTaskCommand(Channel &channel, int taskcommand)
{
    write(channel.Getwfd(), &taskcommand, sizeof(taskcommand));
}  

// 2.通过channel控制子进程
void ctrlProcessOnce(std::vector<Channel> &channels)//一次ctrl
{
    sleep(1);
     // a.选择一个任务
     int taskcommand = SelectTask(); // 任务码，对应任务数组里的下标

     // b.发送一个信道和进程
     int channel_index = NextChannel(channels.size());

     // c.发送任务
     SendTaskCommand(channels[channel_index], taskcommand);

     std::cout << "taskcommand:" << taskcommand << "channel:"
               << channels[channel_index].Getname() << "sub process:" << channels[channel_index].GetProcess() << std::endl;
}
void ctrlProcess(std::vector<Channel>& channels,int times = -1)
{
    if(times > 0)
    {
        while(times--)
        {
            ctrlProcessOnce(channels);
        }
    }
    else
    {
        while (true)
        {
            ctrlProcessOnce(channels);
        }
    }
}


// 3.回收管道和子进程
void CleanUpChannel(std::vector<Channel> &channels)
{
     // a.关闭所有的写端  b.回收子进程
    for(auto &channel : channels)
    {
        channel.ChannelClose();
    }
    for(auto &channel : channels)
    {
        channel.Wait();
    } 
    //这里必须分两个循环来关和接受，如果一个管道有多个写端，等待会因为写端未完全关闭而导致阻塞  
    //例如现在有十个管道，那第一个管道的读端会有十个指向他，必须先将所有的管道关闭，然后再等待
}





//./processpool 5
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage:" << argv[0] << "processnum" << std::endl;
        return 1;
    }
    int num = std::stoi(argv[1]);
    LoadTask();

    std::vector<Channel> channels;
    // 1.创建信道和子进程
    CreateChannelAndSub(num, &channels,work);

    // 2.通过channel控制子进程
    ctrlProcess(channels,10);

    // 3.回收管道和子进程
    CleanUpChannel(channels);
   






    // for test
    // for (auto channel : channels)
    // {
    //     std::cout << "=================================================" << std::endl;
    //     std::cout << channel.Getname() << std::endl;
    //     std::cout << channel.GetProcess() << std::endl;
    //     std::cout << channel.Getwfd() << std::endl;
    // }
    // sleep(100);

    return 0;
}
