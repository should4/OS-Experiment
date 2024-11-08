#include <iostream>
#include "ProcessAndResourceManager.h"

using namespace std;

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

    bool stop = false;
    while (!stop)
    {
        cout << "================================================================" << endl;
        cout << "1. CreatePCB" << endl;
        cout << "2. Request" << endl;
        cout << "3. Release" << endl;
        cout << "4. Show All PCB" << endl;
        cout << "5. Show All RCB" << endl;
        cout << "6. Show PCB" << endl;
        cout << "7. Show RCB" << endl;
        cout << "8. Destroy Process" << endl;
        cout << "9. Tree Process" << endl;
        cout << "10. Timeout" << endl;
        cout << "11. Exit" << endl;
        cout << "================================================================" << endl;
        int choose;
        cin >> choose;
        switch (choose)
        {
        case 1:
        {
            string pid;
            int prio;
            cout << "PCB pid : ";
            cin >> pid;
            cout << "PCB priority(0-Init 1-User 2-System) : ";
            cin >> prio;
            manager->CreatePCB(pid, prio);
            break;
        }
        case 2:
        {
            int id;
            cout << "RCB id : ";
            cin >> id;
            manager->Request(id);
            break;
        }
        case 3:
        {
            int id;
            cout << "RCB id : ";
            cin >> id;
            manager->Release(id);
            break;
        }
        case 4:
        {
            manager->ListAllPCB();
            break;
        }
        case 5:
        {
            manager->ListAllRCB();
            break;
        }
        case 6:
        {
            int id;
            cout << "PCB id : ";
            cin >> id;
            manager->ShowPCB(id);
            break;
        }
        case 7:
        {
            int id;
            cout << "RCB id : ";
            cin >> id;
            manager->ShowRCB(id);
            break;
        }
        case 8:
        {
            int id;
            cout << "Destroy PCB id : " << endl;
            cin >> id;
            manager->DestroyPCB(id);
            break;
        }
        case 9:
        {
            manager->Tree();
            break;
        }
        case 10:
        {
            manager->Timeout();
            break;
        }
        case 11:
        {
            cout << "Exit !" << endl;
            stop = true;
            break;
        }
        default:
            cout << "No such command number !" << endl;
            break;
        }
    }
    return 0;
}