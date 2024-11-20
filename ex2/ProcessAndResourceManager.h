#pragma once
#include "my_struct.h"

#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <set>

using namespace std;

// 进程与资源管理调度类
class ProcessAndResourceManager
{
private:
    int current_id;          // 当前运行的进程ID
    int tick_pid;            //  PID 计数器 防止 pid 重复
    map<int, PCB *> PCBList; // 进程列表 <>
    // <PCB.ID>
    vector<queue<int>> ReadyList; // PCB进程就绪队列 采用固定优先级
    set<int> BlockList;           // PCB 阻塞队列
    // vector<int> RunningList;      // PCB 运行队列

    RCB *_IO; // 代表 IO资源

    map<int, RCB *> RCBList; // 资源进程管理器队列 <资源id，资源结构体指针>

public:
    ProcessAndResourceManager();
    ~ProcessAndResourceManager();

    /**
     * @brief 创建进程,完成进程初始化,然后将进程插入对应的就绪优先级队列中.
     * @param pid 被创建进程的进程名称
     * @param prio  被创建进程的优先级
     */
    void CreatePCB(const string pid, int prio);

    /**
     * @brief 进程调度,将最高优先级的进程从就绪态转换为运行态.
     */
    void Scheduler();

    // 时间片轮转
    // 当前运行进程的运行时间结束,将当前运行进程插入就绪优先级队列,并调度优先级最高的就绪进程运行.
    void Timeout();

    // 进程操作
    // 删除进程
    void DestroyPCB(const int &id);
    void DestroyPCBByPID(const string &pid);

    // 当前运行进程请求资源
    void Request(const int &id);
    void RequestByRID(const string &rid);
    // 当前运行进程释放资源
    void Release(const int &id);
    void ReleaseByRID(const string &rid);

    // 进程数据显示
    void ListAllPCB();
    void ListAllRCB();

    void ShowPCB(const int &id);          // 根据进程id查找进程
    void ShowPCBByPID(const string &pid); // 根据进程pid查找进程
    void ShowRCB(const int &id);          // 根据资源id查找进程
    void ShowRCBByRID(const string &rid); // 根据资源rid查找进程

    void TreePCB(const int &id, const int &level = 0); // 递归显示父子进程
    void Tree();                                       // 以树状形式显示父子进程

    // 进程及资源管理器的升级版 ：IO 资源请求
    void RequestIO();    // 请求 IO 资源，并产生 IO 中断
    void IOCompletion(); // IO 完成
};