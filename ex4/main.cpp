#include <iostream>
#include "MemoryManage.h"

using namespace std;

void debug_test();

int main()
{
    debug_test();

    return 0;
}

void debug_test()
{
    mm_init();
    while (true)
    {
        cout << "1 - Request" << endl;
        cout << "2 - Release" << endl;
        cout << "3 - Set allocation strategy <-> First Fit | Best Fit" << endl;
        cout << "4 - Show current memory uziliazation" << endl;
        cout << "5 - Show Memory" << endl;
        cout << "6 - exit" << endl;
        int choose = 0;
        cin >> choose;
        switch (choose)
        {
        case 1: // Request
        {
            int n;
            int *query_counts = new int;
            (*query_counts) = 0;
            cout << "request size: ";
            cin >> n;
            char *addr = (char *)mm_request(n, query_counts);

            if (addr == nullptr)
            {
                cout << "request failed !" << endl;
            }
            else
            {
                cout << "request successfully" << endl;
                cout << "address : " << addr << endl;
                cout << "query_counts : " << *query_counts << endl;
            }
            break;
        }
        case 2: // Release
        {
            int offset = 0;
            cout << "offset address: ";
            cin >> offset;

            mm_release(get_addr_by_offset(offset));
            break;
        }
        case 3: // Set allocation strategy
        {
            int strat = 0;
            cout << "Allocation Stratagy: 0-FirstFit 1-BestFit: ";
            cin >> strat;
            set_alloc_strat(strat);
            break;
        }
        case 4:
        {
            float perc = ((int)(get_mm_utilization() * 10000) * 1.0 / 100); // 百分数不进位保留 2 位小数
            cout << "Memory Uziliazation: " << perc << "%" << endl;
            break;
        }
        case 5:
        {
            show_mm();
            break;
        }
        case 6:
        {
            exit(0);
        }
        default:
            break;
        }
    }
}
