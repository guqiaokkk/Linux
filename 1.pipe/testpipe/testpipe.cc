#include<iostream>
#include<unistd.h>
#include<cerrno>
#include<cstring>
#include<sys/wait.h>
#include<sys/types.h>

const int size = 1024;
//匿名管道
std::string getOtherMessage()
{
    static int cnt = 0;
    std::string messageid = std::to_string(cnt);
    cnt++;
    pid_t self_id = getpid();
    std::string stringpid = std::to_string(self_id);

    std::string message = "messageid:";
    message += messageid;
    message += "my pid is:";
    message += stringpid;
    
    return message;
}

//子进程进行写入
void SubProcessWrite(int wfd)
{
    std::string message = "linux111bit";
    while(true)
    {
        std::string info = message + getOtherMessage();//这条消息就是我们子进程，发给父进程的消息
        write(wfd,info.c_str(),info.size());//写入管道时，没有写入\0，没必要
        sleep(2);
        
    }

} 


//父进程进行读取
void FatherProcessRead(int rfd)
{
    char inbuffer[size];
    while(true)
    {
        ssize_t n = read(rfd,inbuffer,sizeof(inbuffer));
        if(n > 0)
        {
            inbuffer[n] = 0;
            std::cout<< "father get message:" << inbuffer << std::endl;
            sleep(2);
        }
        else if(n == 0)
        {
            //如果read的返回值是0，表示写端直接关闭了，我们读到了文件的结尾
            std::cout << "client quit,father get return val:" << n << "father quit too" << std::endl;
            break;
        }
        else if(n < 0)
        {
            std::cerr << "read error" << std::endl;
            break;
        }
        //sleep(1);
        //break;

    }
}

 
int main()
{
    //1.创建管道
    int pipefd[2];
    int n = pipe(pipefd);//输出型参数  rfd  wfd

    if(n != 0)
    {
        std::cerr << "errno" << errno << ":" << "errstring:" << strerror(errno) << std::endl;
        return 1;
    }
    //pipefd[0]->读端 r      pipefd[1]->写端 w
    std::cout << "pipedfd[0]:" << pipefd[0] << ",pipefd[i]:" << pipefd[1] << std::endl;
    
    //2.创建子进程
    pid_t id = fork();
    if(id == 0)
    {
        //子进程 -- 这次让他write(可以自定义w/r的)
        //3.关闭不需要的fd
        
        close(pipefd[0]);
        SubProcessWrite(pipefd[1]);
        close(pipefd[1]);
        exit(0);
    }
    //父进程 -- r
    //3.关闭不需要的fd
    
    close(pipefd[1]);
    FatherProcessRead(pipefd[0]);
    
    //std::cout<< "5s,fahter read close" << std::endl;
    //sleep(5);
    
    close(pipefd[0]);


    //int status = 0;
    //pid_t rid = waitpid(id,&status,0);   r关闭，若w一直在写，os会直接关闭这个管道

    pid_t rid = waitpid(id,nullptr,0);
    if(rid > 0)
    {
        
        //std::cout << "wait child process signal" << (status & 0x7f) << std::endl;
        //收到13号信号，相当于进程出现异常
        std::cout << "wait child process done" << std::endl;
    }
    
    return 0;
}