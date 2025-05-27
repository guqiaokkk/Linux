#include<iostream>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>

void hander(int sig)
 {
     std::cout << "get a sig : " << sig << std::endl;
 } 



int main()
{
    int cnt = 1;
    //alarm(1);//  设定1秒的闹钟 ———— 1s ————SIGALRM


    alarm(5);
    sleep(4);
    int n = alarm(0);//alarm(0) 表示取消闹钟，返回值n表示上个闹钟的剩余时间
    std::cout << n << std::endl; 







    sleep(1);
    while(false)
    {
        std::cout << "cnt : " << cnt << std::endl;
        cnt++;
    }
}








// int main()
// {
//     int cnt = 0;
//     signal(SIGABRT,hander);//允许捕捉，但捕捉后还是会终止。 9号信号不允许自定义捕捉
//     while(true)
//     {
//         sleep(2);
//         abort();//发送6号信号终止
//         //raise(3);//给自己发信号
//     }
// }



// //./mykill 2 1234(pid)
// int main(int argc, char* argv[])
// {
//     if(argc != 3)
//     {
//         std::cout << "Usage : " << argv[0] << "   signum  pid " << std::endl;
//         return 1; 
//     }
//     pid_t pid = std::stoi(argv[2]);
//     int signum = std::stoi(argv[1]);
//     kill(pid,signum);
// }

// int main()
// {
//     //对信号的自定义捕捉，只要捕捉一次，后续会一直有效
//     // signal(2,hander);//默认的是CTRL + c
//     // signal(3,hander);
//     // signal(4,hander);
//     // signal(5,hander);

//     while (true)
//     {
//         std::cout << "hello kjy , pid :" << getpid() << std::endl;
//         sleep(1);
//     }
//     return 0;
// }
