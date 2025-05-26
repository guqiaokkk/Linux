#ifndef __SHM_HPP__
#define __SHM_HPP__

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <cerrno>
#include <cstdio>
#include<unistd.h>
#include<cstring>

const int gShmsize = 4096; // 建议4096*n,如果设4097，会给你4096*2，但你只能用4097  
const int gproj_id = 0x66;
const std::string gpathname = "/home/kkkjy";//要求路径是有效的，不然会ftok 返回 -1，这表明生成键值（key）失败了
const int gCreater = 1;
const int gUser = 2;

class Shm
{
private:


    key_t GetCommkey() // 生成key
    {
        key_t k = ftok(_pathname.c_str(), _proj_id);
        if (k < 0)
        {
            perror("ftok");
        }
        return k;
    }



    int GetShmHelper(key_t key, int size, int flag) // 根据key和flag创建共享内存，返回shmid
    {
        int shmid = shmget(key, size, flag);
        if (shmid < 0)
        {
            perror("shmget");
        }
        return shmid;
    }

    std::string RoleToString(int who)
    {
        if(who == gCreater) return "Creater";
        else if(who == gUser) return "User";
        else return "None"; 
    }


    void *AttachShm()//挂接共享内存
    {
        if(_addrshm != nullptr) DetachShm(_addrshm);
        void* shmaddr = shmat(_shmid,nullptr,0);// 返回的是共享内存的起始地址
        if(shmaddr == nullptr)
        {
            perror("shmat");
        }
        std::cout << "who : " << RoleToString(_who) << "attach shm..." <<std::endl;
        return shmaddr;
    }

    void DetachShm(void* shmadrr)//去除和共享内存关联
    {
        if(shmadrr == nullptr)return;
        shmdt(shmadrr);
    }

public:

    Shm(const std::string &pathname, int proj_id, int who)
        : _pathname(pathname), _proj_id(proj_id), _who(who),_addrshm(nullptr)
    {
        _key = GetCommkey();

        if (_who == gCreater)
            GetShmCreater();
        else if (_who == gUser)
            GetShmUser();

        _addrshm = AttachShm();
        
            std::cout << "shmid :" << _shmid << std::endl;
        std::cout << "key :" << ToHex(_key) << std::endl;
    }



    ~Shm()
     {
        if(_who == gCreater)
        {
            int res = shmctl(_shmid,IPC_RMID,nullptr);
        }
        std::cout << "shm remove done" << std::endl;
     }

    std::string ToHex(key_t key) // 转十六进制
    {
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "0x%x", key);
        return buffer;
    }



    bool GetShmCreater()
    {
        if (_who == gCreater)
        {
            _shmid = GetShmHelper(_key, gShmsize, IPC_CREAT | IPC_EXCL | 0666);
            if (_shmid >= 0)
            {
                return true;
                std::cout << "shm create done" << std::endl;
            }
        }
        return false;
    }


    bool GetShmUser()
    {
        if (_who == gUser)
        {
            _shmid = GetShmHelper(_key, gShmsize, IPC_CREAT | 0666);
            if (_shmid >= 0)
             {   
                std::cout << "shm get done" << std::endl;
                return true;
             }
        }
        return false;
    }

    


    void* Addr()
    {
        return _addrshm;
    }

    void Zero()
    {
        if(_addrshm)
        {
            memset(_addrshm,0,gShmsize);
        }
    }

    void DebugShm()
    {
        struct shmid_ds ds;
        int n = shmctl(_shmid,IPC_STAT,&ds);//得到共享内存属性的获取
        if(n < 0)return;
        std::cout << "ds.shm_perm._key : " << ToHex(ds.shm_perm.__key) << std::endl;
        std::cout << "ds.shm_nattch : " << ds.shm_nattch << std::endl; 
    }



private:
    key_t _key;
    int _shmid;
    std::string _pathname;
    int _proj_id;
    int _who;
    void* _addrshm;
};

#endif