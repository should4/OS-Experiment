#include <iostream>
#include <sstream>
#include "ProcessAndResourceManager.h"

using namespace std;

vector<string> split(string s);
int str2int(string str);

int main(int argc, char **argv)
{
    ProcessAndResourceManager *manager = new ProcessAndResourceManager();

    // manager->ListAllPCB();
    // manager->ListAllRCB();
    // manager->ShowInform(0);

    // if (argc == 2)
    // {
    //     manager->ShowInform(atoi(argv[1]));
    // }

    // manager->CreatePCB("Process A", USER_PRIORITY);
    // manager->CreatePCB("Process B", USER_PRIORITY);
    // manager->Request(0);

    // manager->CreatePCB("Process C", SYSTEM_PRIORITY);
    // manager->CreatePCB("Process D", USER_PRIORITY);

    // manager->ListAllPCB();
    // manager->ListAllRCB();
    // manager->ShowInform(0);
    // manager->ShowInform(1);
    // manager->ShowInform(2);
    // manager->ShowInform(3);
    // manager->ShowInform(4);

    // 交互方式 1 : 命令表格化显示
    // bool stop = false;
    // while (!stop)
    // {
    //     cout << "================================================================" << endl;
    //     cout << "1. CreatePCB" << endl;
    //     cout << "2. Request" << endl;
    //     cout << "3. Release" << endl;
    //     cout << "4. Show All PCB" << endl;
    //     cout << "5. Show All RCB" << endl;
    //     cout << "6. Show PCB" << endl;
    //     cout << "7. Show RCB" << endl;
    //     cout << "8. Destroy Process" << endl;
    //     cout << "9. Tree Process" << endl;
    //     cout << "10. Timeout" << endl;
    //     cout << "11. Request IO" << endl;
    //     cout << "12. IO Completion" << endl;
    //     cout << "13. Exit" << endl;
    //     cout << "================================================================" << endl;
    //     int choose;
    //     cin >> choose;
    //     switch (choose)
    //     {
    //     case 1:
    //     {
    //         string pid;
    //         int prio;
    //         cout << "PCB pid : ";
    //         cin >> pid;
    //         cout << "PCB priority(0-Init 1-User 2-System) : ";
    //         cin >> prio;
    //         manager->CreatePCB(pid, prio);
    //         break;
    //     }
    //     case 2:
    //     {
    //         int id;
    //         cout << "RCB id : ";
    //         cin >> id;
    //         manager->Request(id);
    //         break;
    //     }
    //     case 3:
    //     {
    //         int id;
    //         cout << "RCB id : ";
    //         cin >> id;
    //         manager->Release(id);
    //         break;
    //     }
    //     case 4:
    //     {
    //         manager->ListAllPCB();
    //         break;
    //     }
    //     case 5:
    //     {
    //         manager->ListAllRCB();
    //         break;
    //     }
    //     case 6:
    //     {
    //         int id;
    //         cout << "PCB id : ";
    //         cin >> id;
    //         manager->ShowPCB(id);
    //         break;
    //     }
    //     case 7:
    //     {
    //         int id;
    //         cout << "RCB id : ";
    //         cin >> id;
    //         manager->ShowRCB(id);
    //         break;
    //     }
    //     case 8:
    //     {
    //         int id;
    //         cout << "Destroy PCB id : " << endl;
    //         cin >> id;
    //         manager->DestroyPCB(id);
    //         break;
    //     }
    //     case 9:
    //     {
    //         manager->Tree();
    //         break;
    //     }
    //     case 10:
    //     {
    //         manager->Timeout();
    //         break;
    //     }
    //     case 11:
    //     {
    //         manager->RequestIO();
    //         break;
    //     }
    //     case 12:
    //     {
    //         manager->IOCompletion();
    //         break;
    //     }
    //     case 13:
    //     {
    //         cout << "Exit !" << endl;
    //         stop = true;
    //         break;
    //     }
    //     default:
    //         cout << "No such command number !" << endl;
    //         break;
    //     }
    // }

    //     cout << "1. CreatePCB" << endl;
    //     cout << "2. Request" << endl;
    //     cout << "3. Release" << endl;
    //     cout << "4. Show All PCB" << endl;
    //     cout << "5. Show All RCB" << endl;
    //     cout << "6. Show PCB" << endl;
    //     cout << "7. Show RCB" << endl;
    //     cout << "8. Destroy Process" << endl;
    //     cout << "9. Tree Process" << endl;
    //     cout << "10. Timeout" << endl;
    //     cout << "11. Request IO" << endl;
    //     cout << "12. IO Completion" << endl;
    //     cout << "13. Exit" << endl;

    // 交互方式 2 ： 采用固定格式命令的格式 使用
    bool stop = false;
    while (!stop)
    {
        cout << "@OSmanager: ";
        // 获取一行输入信息
        string line_str;
        getline(cin, line_str);
        vector<string> split_str = split(line_str);

        if (split_str.empty())
        {
            cout << "input empty !" << endl;
            continue;
        }

        // 对命令进行语法分析
        if (split_str[0] == "cr") // CreatePCB
        {
            if (split_str.size() != 3)
            {
                cout << "command error : " << endl;
                cout << "create command : cr [PID] [Priority]" << endl;
                continue;
            }
            manager->CreatePCB(split_str[1], str2int(split_str[2])); // 此处对于优先级的 str->int 的转换可能不合法 存在 bug
        }
        else if (split_str[0] == "req") // Request
        {
            if (split_str.size() != 2)
            {
                cout << "command error : " << endl;
                cout << "request command : req [RID]" << endl;
                continue;
            }
            manager->RequestByRID(split_str[1]);
        }
        else if (split_str[0] == "rel") // Release
        {
            if (split_str.size() != 2)
            {
                cout << "command error : " << endl;
                cout << "release command : rel [RID]" << endl;
                continue;
            }
            manager->ReleaseByRID(split_str[1]);
        }
        else if (split_str[0] == "list") // List
        {
            if (split_str.size() != 2)
            {
                cout << "command error : " << endl;
                cout << "list command : list PCB|RCB" << endl;
                continue;
            }

            if (split_str[1] == "PCB")
            {
                manager->ListAllPCB();
            }
            else if (split_str[1] == "RCB")
            {
                manager->ListAllRCB();
            }
            else
            {
                cout << "command error : " << endl;
                cout << "list command : list PCB|RCB" << endl;
                continue;
            }
        }
        else if (split_str[0] == "show")
        {
            if (split_str.size() != 3)
            {
                cout << "command error : " << endl;
                cout << "show command : show PCB|RCB [PID]|[RID]" << endl;
                continue;
            }

            if (split_str[1] == "PCB")
            {
                manager->ShowPCBByPID(split_str[2]);
            }
            else if (split_str[1] == "RCB")
            {
                manager->ShowRCBByRID(split_str[2]);
            }
            else
            {
                cout << "command error : " << endl;
                cout << "show command : show PCB|RCB [PID]|[RID]" << endl;
                continue;
            }
        }
        else if (split_str[0] == "de") // Destroy PCB
        {
            if (split_str.size() != 2)
            {
                cout << "command error : " << endl;
                cout << "show command : del [PID]" << endl;
                continue;
            }
            manager->DestroyPCBByPID(split_str[1]);
        }
        else if (split_str[0] == "tree") // Tree
        {
            manager->Tree();
        }
        else if (split_str[0] == "to") // Timeout
        {
            manager->Timeout();
        }
        else if (split_str[0] == "ioreq")
        {
            manager->RequestIO();
        }
        else if (split_str[0] == "ioc") // IO Completion
        {
            manager->IOCompletion();
        }
        else if (split_str[0] == "q") // quit 退出程序
        {
            stop = true;
        }
    }

    return 0;
}

int str2int(string str)
{
    stringstream ss;
    ss << str;
    int num;
    ss >> num;
    return num;
}

vector<string> split(string s)
{
    vector<string> split_s;
    string::size_type l = 0, r;
    while (l < s.size())
    {
        if (s.find(" ", l) == string::npos)
        {
            split_s.emplace_back(s.substr(l, s.size() - l));
            break;
        }
        r = s.find(" ", l);
        split_s.emplace_back(s.substr(l, r - l));
        l = r + 1;
    }
    return split_s;
}