
# 实验二  进程调度与资源管理

## 实验二参考
- [OS_Sched_Experience](https://github.com/ALIEX7664/OS_Sched_Experience/tree/master)

- [C++中引用（&）的用法和拷贝/赋值函数的区别](https://blog.csdn.net/KingCat666/article/details/44852565)

## Bugs

### 1. 未使用引用类型而导致未改变希望改变的变量

```cpp
    // auto childs = _pcb->Parent->Childs; // bug here: 未使用引用类型
    auto &childs = _pcb->Parent->Childs; // sovled

    for (std::vector<PCB *>::iterator itr = childs.begin(); itr != childs.end(); ++itr)
    {
        if ((*itr)->ID == _pcb->ID)
        {
            // cout << "pre_size = " << childs.size() << endl;
            childs.erase(itr); // 存在bug
            // cout << "cur_size = " << childs.size() << endl;
            // cout << "erase child from father " << (*itr)->PID << endl;
            break;
        }
    }
```