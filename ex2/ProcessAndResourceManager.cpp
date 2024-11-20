#include "ProcessAndResourceManager.h"

ProcessAndResourceManager::ProcessAndResourceManager() : current_id(-1),
                                                         tick_pid(0),
                                                         ReadyList(), BlockList(), RCBList(), PCBList() //, RunningList(),
{
    // 初始化 PCBList,创建 Init 进程
    PCB *init_pcb = new PCB(tick_pid++, "init", INIT_PRIORITY, READY);
    PCBList.emplace(pair<int, PCB *>(init_pcb->ID, init_pcb));

    // 初始化 就绪队列，将 init 进程插入 ReadyList 中
    ReadyList.resize(PRIO_NUM);
    ReadyList[INIT_PRIORITY].emplace(init_pcb->ID);

    // 初始化资源列表
    RCB *r0 = new RCB(0, "R0");
    RCB *r1 = new RCB(1, "R1");
    RCB *r2 = new RCB(2, "R2");
    RCB *r3 = new RCB(3, "R3");
    RCBList.insert(pair<int, RCB *>(0, r0));
    RCBList.insert(pair<int, RCB *>(1, r1));
    RCBList.insert(pair<int, RCB *>(2, r2));
    RCBList.insert(pair<int, RCB *>(3, r3));

    // 初始化 IO 资源
    _IO = new RCB(4, "IO");

    cout << "ProcessAndResourceManager init" << endl;

    // // 将 init 进程从就绪队列中加载到正在运行的队列中
    // ReadyList[INIT_PRIORITY].pop();
    // // RunningList.emplace_back(init_pcb->ID);
    // current_id = init_pcb->ID;
    // init_pcb->state = RUN;
    // cout << "init process start running" << endl;

    Scheduler();
}

ProcessAndResourceManager::~ProcessAndResourceManager()
{
    // 将所有容器中的指针清空
}

void ProcessAndResourceManager::CreatePCB(const string pid, int prio)
{
    // 判断 优先级是否合法
    if (prio < 0 || prio > 2)
    {
        cout << "priority error,no such priority" << endl;
        return;
    }

    // 建立 _pcb 结构体指针
    PCB *_pcb = new PCB(tick_pid++, pid, prio, READY, PCBList.at(current_id)); // 父进程为当前正在运行的进程

    // 把新创建的进程添加到当前进程的子进程列表中
    PCBList.at(current_id)->Childs.emplace_back(_pcb);

    // 把这个新的 pcb 插入 PCBList 和 ReadyList
    PCBList.emplace(pair<int, PCB *>(_pcb->ID, _pcb));
    ReadyList[prio].emplace(_pcb->ID);

    // 进行进行调度
    Scheduler();
}

void ProcessAndResourceManager::DestroyPCB(const int &id)
{
    cout << "destroy :" << id << endl;
    // 判断 id 是否合理
    if (PCBList.find(id) == PCBList.end())
    {
        cout << "No such id in PCBList" << endl;
        return;
    }
    PCB *_pcb = PCBList[id];

    // 删除进程的步骤：
    // [1] 释放当前进程所占用的资源列表，并判断是否有其他进程阻塞到该资源上，如果有，则从阻塞队列中取出一个分配
    for (const auto &itr : _pcb->OcpResList)
    {
        itr.second->state = FREE;
        // 如果该资源的进程阻塞队列不为空
        if (!itr.second->BlockList.empty())
        {
            // 切换该进程状态
            PCB *_blk_pcb = itr.second->BlockList.front();
            itr.second->BlockList.pop();

            _blk_pcb->state = READY;
            _blk_pcb->OcpResList.emplace(itr.first, itr.second);
            BlockList.erase(_blk_pcb->ID);
            ReadyList[_blk_pcb->priority].emplace(_blk_pcb->ID);
        }
    }

    // [2] 判断当前进程所处状态
    switch (_pcb->state)
    {
    case 1: // READY
    {
        // [2.1] 如果处于就绪状态，则将该进程从就绪队列中去掉

        // 利用出入队列将就绪队列中该进程去掉
        queue<int> _que_in;
        queue<int> _que_out(ReadyList[_pcb->priority]);
        while (!_que_out.empty())
        {
            if (_que_out.front() == _pcb->ID)
            {
                _que_out.pop();
            }
            _que_in.emplace(_que_out.front());
            _que_out.pop();
        }
        ReadyList[_pcb->priority] = _que_in;

        break;
    }
    case 2: // RUN
    {
        current_id = -1;
        break;
    }
    case 3: //  BLOCK
    {
        // [2.3] 如果处于阻塞状态，则将该进程从阻塞列表中去掉，并将该进程阻塞资源rcb对象中的阻塞队列中的该进程去掉
        BlockList.erase(_pcb->ID);

        for (const auto &itr : RCBList)
        {
            // 如果当前资源已经被占用且阻塞进程队列不为空，则判断当前进程是否在它的阻塞列表中
            if (itr.second->state == ALLOCATE && !itr.second->BlockList.empty())
            {
                queue<PCB *> _que_in;
                queue<PCB *> _que_out = itr.second->BlockList;

                while (!_que_out.empty())
                {
                    if (_que_out.front()->ID == _pcb->ID)
                    {
                        _que_out.pop();
                    }
                    _que_in.emplace(_que_out.front());
                    _que_out.pop();
                }
                // 只有在修改的地方在进行重新赋值
                if (_que_in.size() != itr.second->BlockList.size())
                {
                    itr.second->BlockList = _que_in;
                }

                break;
            }
        }
        break;
    }
    default:
        break;
    }

    // [3] 将该进程的子进程的父进程，设置为该进程的父进程,并在该父进程中添加子进程s
    if (!_pcb->Childs.empty())
    {
        for (const auto &_child : _pcb->Childs)
        {
            _child->Parent = _pcb->Parent;
            _pcb->Parent->Childs.emplace_back(_child);
        }
    }
    // [5] 将该父进程的子进程列表中删除该进程
    auto childs = _pcb->Parent->Childs;
    for (std::vector<PCB *>::iterator itr = childs.begin(); itr != childs.end(); ++itr)
    {
        if ((*itr)->ID == _pcb->ID)
        {
            // cout << "pre_size = " << childs.size() << endl;
            childs.erase(itr); // 存在bug
            // cout << "cur_size = " << childs.size() << endl;
            cout << "erase child from father " << _pcb->PID << endl;
            break;
        }
    }
    // [4] 将该 PCB 从 PCBList中移除
    PCBList.erase(_pcb->ID);
    // [5] 释放该 pcb 内存
    delete _pcb;
    // [6] 重新调度
    Scheduler();
}
void ProcessAndResourceManager::DestroyPCBByPID(const string &pid)
{
    for (const auto &itr : PCBList)
    {
        if (itr.second->PID == pid)
        {
            cout << "pid " << pid << endl;
            DestroyPCB(itr.first);
            break;
        }
    }
}

void ProcessAndResourceManager::Request(const int &id)
{
    // 当前运行进程请求资源
    // [1] 判断是否存在该资源
    if (RCBList.find(id) == RCBList.end())
    {
        cout << "No such id in RCBList" << endl;
        return;
    }
    // 判断该资源是不是 IO 资源，Request 函数不处理 IO资源中断，由 Request_IO 函数处理
    if (RCBList[id]->RID.find("IO") != std::string::npos)
    {
        RequestIO();
        return;
    }
    // [2] 判断该资源是否可用
    RCB *_rcb = RCBList.at(id);
    if (_rcb->state == FREE)
    {
        // [2.1] 如果可用，则将该资源添加到当前运行进程的占用资源列表中，并将该资源状态设置为 已被分配
        _rcb->state = ALLOCATE;
        PCBList[current_id]->OcpResList.emplace(pair<int, RCB *>(_rcb->ID, _rcb));
    }
    else if (_rcb->state == ALLOCATE)
    {
        // [2.2] 如果不可用，则将当前运行进程阻塞，向该资源的阻塞进程队列中添加当前运行进程，并重新调度.
        PCBList[current_id]->state = BLOCK;
        BlockList.emplace(current_id);
        _rcb->BlockList.emplace(PCBList[current_id]);

        cout << "Block " << PCBList[current_id]->PID << " on " << _rcb->RID << endl;

        current_id = -1;
        Scheduler();
    }
}

void ProcessAndResourceManager::RequestByRID(const string &rid)
{
    for (const auto &itr : RCBList)
    {
        if (itr.second->RID == rid)
        {
            Request(itr.first);
        }
    }
}

void ProcessAndResourceManager::Release(const int &id)
{
    // 释放资源步骤
    // [1] 判断当前运行进程中是否占用该资源
    PCB *_cur_pcb = PCBList[current_id];
    if (_cur_pcb->OcpResList.find(id) == _cur_pcb->OcpResList.end())
    {
        cout << "Current running process do not occupy resource " << id << "as id" << endl;
        return;
    }

    // [2] 如果占用该资源，则将该资源从占用资源列表中移除，并将该资源状态设置为 空闲
    RCB *_rcb = _cur_pcb->OcpResList.at(id);
    _cur_pcb->OcpResList.erase(id);
    _rcb->state = FREE;

    // [3] 查看该资源的阻塞进程队列是否为空，
    // 若不为空，则将队首进程从 Block 状态设置为 Ready,并将该资源添加到该进程的占用资源列表中，并重新调度
    if (!_rcb->BlockList.empty())
    {
        PCB *_blk_pcb = _rcb->BlockList.front();
        _rcb->BlockList.pop();
        BlockList.erase(_blk_pcb->ID); // 将该进程从阻塞队列中转移到就绪队列中
        ReadyList[_blk_pcb->priority].emplace(_blk_pcb->ID);

        _blk_pcb->state = READY;
        _blk_pcb->OcpResList.emplace(_rcb->ID, _rcb);

        // 重新调度
        Scheduler();
    }
}

void ProcessAndResourceManager::ReleaseByRID(const string &rid)
{
    for (const auto &itr : RCBList)
    {
        if (itr.second->RID == rid)
        {
            Release(itr.first);
        }
    }
}
void ProcessAndResourceManager::Scheduler()
{
    // 遍历 ReadyList 找出第优先级高于当前正在运行进程 且 最早到来的进程
    cout << "开始调度...." << endl;
    // 判断当前是否有正在运行的进程
    int cur_prio = -1;
    if (current_id != -1)
    {
        // 获取当前正在运行的进程的优先级
        cur_prio = PCBList.at(current_id)->priority;
    }

    // 遍历策略：从最高优先级的队列开始遍历，遍历到优先级高于当前运行进程队列
    for (int i = SYSTEM_PRIORITY; i > cur_prio; --i)
    {
        // 如果当前队列不为空
        if (!ReadyList[i].empty())
        {
            int new_id = ReadyList[i].front();
            ReadyList[i].pop();
            PCBList[new_id]->state = RUN; // 修改替换进程的状态为运行

            if (current_id != -1)
            {
                // 如果存在正在运行的进程
                cout << "进程 " << PCBList[current_id]->PID << " 被进程 " << PCBList[new_id]->PID << " 替换了" << endl;
                // 修改当前正在运行进程的状态为 READY 并将其插入就绪队列
                PCBList[current_id]->state = READY;
                ReadyList[cur_prio].push(current_id);
            }
            else
            {
                cout << "进程 : " << PCBList[new_id]->PID << " 开始运行" << endl;
            }

            // 迭代当前正在运行的进程
            current_id = new_id;
            break;
        }
    }
    cout << "调度完成...." << endl;
}

void ProcessAndResourceManager::Timeout()

{
    if (current_id != -1)
    {
        PCB *_pcb = PCBList[current_id];
        _pcb->state = READY;
        ReadyList[_pcb->priority].emplace(_pcb->ID);
        current_id = -1;

        cout << "Time out : " << _pcb->PID << endl;
        Scheduler();
    }
}
void ProcessAndResourceManager::ListAllPCB()
{
    cout << "------------ListAllPCB----------------" << endl;
    // 输出正在运行的进程信息
    cout << "|      running     |" << endl;
    cout << "| PID | ID | state |" << endl;
    cout << "| " << PCBList[current_id]->PID << " | " << PCBList[current_id]->ID << " | "
         << PCBList[current_id]->state << " |" << endl;

    // 输出 ReadyList 中的进程
    cout << "|      ready     |" << endl;
    int level = SYSTEM_PRIORITY;
    while (level >= INIT_PRIORITY)
    {
        cout << "|    priority " << level << "   |" << endl;
        cout << "| PID | ID | state |" << endl;
        auto cp_queue = ReadyList[level];
        for (; !cp_queue.empty(); cp_queue.pop())
        {
            auto id = cp_queue.front();
            cout << "| " << PCBList[id]->PID << " | " << PCBList[id]->ID << " | "
                 << PCBList[id]->state << " |" << endl;
        }
        --level;
    }

    // 输出 BlockList 中的进程
    cout << "|      block     |" << endl;
    for (const auto &id : BlockList)
    {
        cout << "| " << PCBList[id]->PID << " | " << PCBList[id]->ID << " | "
             << PCBList[id]->state << " |" << endl;
    }

    cout << "---------------------------------------" << endl;
}
void ProcessAndResourceManager::ListAllRCB()
{
    cout << "------------ListAllRCB----------------" << endl;
    cout << "|      RESOURCES   |" << endl;
    cout << "| RID | ID | state |" << endl;
    for (const auto &rcb : RCBList)
    {
        cout << "| " << rcb.second->RID << " | " << rcb.second->ID << " | " << rcb.second->state << " |" << endl;
    }
    cout << "---------------------------------------" << endl;
}
void ProcessAndResourceManager::ShowPCB(const int &id)
{
    cout << "--------------ShowPCB------------------" << endl;
    // 在 PCBLIST 中查找该进程id 所对应的PCB结构体指针
    if (PCBList.find(id) == PCBList.end())
    {
        cout << "No such id in PCBList" << endl;
        return;
    }
    PCB *_pcb = PCBList.at(id);

    // 输出 pcb 信息
    cout << "pid: " << _pcb->PID << endl;
    cout << "priority: " << _pcb->priority << endl;
    cout << "state: " << _pcb->state << endl;
    cout << "parent: " << (_pcb->Parent == nullptr ? "No parent" : _pcb->Parent->PID) << endl;
    cout << "childs {" << endl;
    for (const auto &child : _pcb->Childs)
    {
        cout << "   " << child->PID << " ," << endl;
    }
    cout << "}" << endl;

    // 输出所占用资源列表
    cout << "occupied resources {" << endl;
    for (const auto &res : _pcb->OcpResList)
    {
        cout << "   " << RCBList[res.first]->RID << " ," << endl;
    }
    cout << "}" << endl;
    cout << "---------------------------------------" << endl;
}
void ProcessAndResourceManager::ShowPCBByPID(const string &pid)
{
    // 遍历 PCB 列表查找
    for (const auto &itr : PCBList)
    {
        if (itr.second->PID == pid)
        {
            ShowPCB(itr.first);
        }
    }
}

void ProcessAndResourceManager::ShowRCB(const int &id)
{
    cout << "--------------ShowRCB------------------" << endl;
    // 在 RCBLIST 中查找该进程id 所对应的RCB结构体指针
    if (RCBList.find(id) == RCBList.end())
    {
        cout << "No such id in PCBList" << endl;
        return;
    }
    RCB *_rcb = RCBList.at(id);

    // 输出 rcb 信息
    cout << "RID: " << _rcb->RID << endl;
    cout << "state: " << _rcb->state << endl;
    cout << "BlockList {" << endl;
    auto _cp_blk_list = _rcb->BlockList;
    for (; !_cp_blk_list.empty(); _cp_blk_list.pop())
    {
        cout << " " << _cp_blk_list.front()->PID << endl;
    }
    cout << "}" << endl;
    cout << "---------------------------------------" << endl;
}

void ProcessAndResourceManager::ShowRCBByRID(const string &rid)
{
    // 遍历 RCB 列表查找
    for (const auto &itr : RCBList)
    {
        if (itr.second->RID == rid)
        {
            ShowPCB(itr.first);
        }
    }
}
void ProcessAndResourceManager::TreePCB(const int &id, const int &level)
{
    if (PCBList.find(id) == PCBList.end())
    {
        return;
    }
    PCB *_pcb = PCBList[id];
    // 添加缩进来表示层级
    string indent = "";
    for (int i = 0; i < 4 * level; ++i)
    {
        indent += " ";
    }

    // 输出当前 PCB
    cout << indent << "|-- " << _pcb->PID << endl;

    // 递归输出子进程
    if (!_pcb->Childs.empty())
    {
        for (const auto &child : _pcb->Childs)
        {
            TreePCB(child->ID, level + 1);
        }
    }

    return;
}

void ProcessAndResourceManager::Tree()
{
    // 将 PCB 的父子关系以树状图的形式显示出来
    cout << "---------------TreeRCB------------------" << endl;
    TreePCB(0);
    cout << "---------------------------------------" << endl;
}

void ProcessAndResourceManager::RequestIO()
{
    // 当前运行进程请求 IO 中断
    cout << "请求 IO 中断" << endl;
    // [1] 将当前运行进程状态切换到阻塞态并加入阻塞队列中
    PCBList[current_id]->state = BLOCK;
    BlockList.emplace(current_id);
    // [2] 将当前进程id 添加到 IO 资源的阻塞队列中,等待 IO 访问完成
    _IO->BlockList.emplace(PCBList[current_id]);
    current_id = -1;
    // [3] 重新调度
    Scheduler();
}

void ProcessAndResourceManager::IOCompletion()
{
    // 判断 IO 阻塞队列中是否有进程
    if (_IO->BlockList.empty())
    {
        cout << "IO waitting list empty,no proccess to be awake!" << endl;
        return;
    }
    // IO 中断完成
    cout << "IO 中断完成,";
    // [1] 当 IO 中断完成后会将 IO 资源阻塞进程队列中的进程唤醒 进程状态由: 阻塞 -> 就绪
    PCB *_wake_pcb = _IO->BlockList.front();
    _IO->BlockList.pop();
    _wake_pcb->state = READY;
    BlockList.erase(_wake_pcb->ID);
    ReadyList[_wake_pcb->priority].emplace(_wake_pcb->ID); // 放入就绪队列
    cout << "进程 " << _wake_pcb->PID << " 被唤醒!" << endl;
    // [2] 重新调度
    Scheduler();
}