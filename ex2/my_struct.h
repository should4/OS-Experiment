#pragma once

#include <map>
#include <queue>
#include <string>
#include <vector>

using namespace std;

struct PCB; // 进程控制类
struct RCB; // 资源控制类

/*
**********************************************************************************************************
*									Status Code - 状态码
**********************************************************************************************************
*/

// 进程运行状态
#define READY 1
#define RUN 2
#define BLOCK 3

// 资源状态
#define FREE 1
#define ALLOCATE 2

// 优先级
#define INIT_PRIORITY 0
#define USER_PRIORITY 1
#define SYSTEM_PRIORITY 2

#define PRIO_NUM 3 // 优先级数量

/*
*********************************************************************************************************
*										Struct - 结构体
*********************************************************************************************************
*/

// 进程 - PCB
struct PCB
{
    int ID;       // 进程ID，进程的唯一标识，供其他进程引用该进程
    string PID;   // 进程名称
    int priority; // 优先级
    int state;    // 运行状态 READY-就绪 RUN-运行 BLOCK-阻塞

    PCB *Parent;          // 指向父进程
    vector<PCB *> Childs; // 指向多个子进程

    map<int, RCB *> OcpResList; // 已占用资源队列 <资源id,资源结构体指针>

    PCB() : ID(-1), PID(""), priority(-1), state(-1), Parent(nullptr), Childs({}), OcpResList({}) {};
    PCB(int _ID, string _PID, int _priority, int _state, PCB *_parent = nullptr) : ID(_ID), PID(_PID), priority(_priority), state(_state), Parent(_parent), OcpResList() {};
    ~PCB()
    {
        OcpResList.clear();
        Childs.clear();
    };
};

// 资源管理器 - RCB
struct RCB
{
    int ID;                 // 资源的唯一标识，由进程引用
    string RID;             // 资源名称
    int state;              // FREE-未被占用  ALLOCATE 被占用
    queue<PCB *> BlockList; // 阻塞队列,记录阻塞于该资源的PCB （等待队列——是被本资源阻塞的进程链表，本资源正被其他所有资源都设定为静态数据，系统启动时初始化。）

    RCB() : ID(-1), RID(""), state(-1) {};
    RCB(int _id, string _rid, int _state = FREE) : ID(_id), RID(_rid), state(_state) {};
};