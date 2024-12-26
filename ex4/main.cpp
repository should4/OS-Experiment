/**
 * @file main.cpp
 * @author xuanrong gu @3085995438@qq.com
 * @brief
 * @version 0.1
 * @date 2024-12-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <iostream>
#include "MemoryManage.h"
#include <time.h>
#include <list> // 请求队列

using namespace std;

void debug_test();                                             // 调试程序功能
void simulate(list<int> request_que, int strategy, int steps); // 模拟实验 strategy:表示动态分区分配算法 steps:表示模拟次数

list<int> request_generator(int num); // 请求生成器

int main()
{
    // 开始产生随机数种子
    srand(time(nullptr));

    while (true)
    {
        int mode;
        cin >> mode;
        switch (mode)
        {
        case 0:
            debug_test();
            break;
        case 1:
        {
            int steps;
            cout << "input steps: ";
            cin >> steps;
            cout << "generate size: " << steps * MEM_SIZE << endl;
            list<int> request_que = request_generator(steps * MEM_SIZE);

            cout << "----------First Fit----------" << endl;
            simulate(request_que, FIRST_FIT_STRATEGY, steps);
            cout << endl;

            cout << "----------Best Fit----------" << endl;
            simulate(request_que, BEST_FIT_STRATEGY, steps);
            cout << endl;
            break;
        }

        default:
            break;
        }
    }

    return 0;
}

list<int> request_generator(int num)
{
    list<int> request_que = {};
    for (int i = 0; i < num; ++i)
    {
        int r_size = rand() % MEM_SIZE + 1;
        request_que.emplace_back(r_size);
    }
    return request_que;
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
        cout << "6 - release by chunk index" << endl;
        cout << "7 - exit" << endl;
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
            if (get_mm_alloc_chunk_num() == 0)
                continue;
            int rel_index = rand() % get_mm_alloc_chunk_num();
            cout << "rel_index: " << rel_index << endl;
            cout << "get_alloc_addr_by_index(rel_index): " << get_alloc_addr_by_index(rel_index) << endl;
            mm_release(get_alloc_addr_by_index(rel_index));
            break;
        }
        case 7:
        {
            exit(0);
        }
        default:
            break;
        }
    }
}

void simulate(list<int> request_que, int strategy, int steps)
{
    // 初始化内存
    mm_init();

    // 设置分配策略
    set_alloc_strat(strategy);

    int *query_counts = new int(0); // 模拟的遍历链表节点总次数
    float sum_mm_utilization = 0;
    int request_num = 0;

    for (int i = 0; i < steps; ++i)
    {
        // 执行请求命令
        while (true)
        {
            if (request_que.empty()) // 如果请求队列当前为空，则随机生成一个请求
            {
                // 随机生成策略: 请求内存大小小于总内存大小
                // cout << "add new" << endl;
                int r_size = rand() % MEM_SIZE + 1;
                request_que.emplace_back(r_size);
            }

            // 从请求队列中获取第一个元素
            int n = request_que.front();
            char *addr = (char *)mm_request(n, query_counts);
            // cout << "request size: " << n << endl;

            // 若请求失败，则放弃
            if (!addr)
            {
                // show_mm();
                break;
            }

            request_que.pop_front(); // 完成请求，移出队列
            request_num++;
        }

        // 记录当前内存利用率
        sum_mm_utilization += get_mm_utilization();
        // cout << "sum_mm_uzilization" << sum_mm_utilization << endl;
        // 输出每轮的性能参数
        // cout << "Memory Utilization round" << i << " :" << ((int)(get_mm_utilization() * 10000) * 1.0 / 100) << "%" << endl;
        // 随机生成一个释放内存请求
        // 随即策略 [1]：随机选择一个已分配区块
        if (get_mm_alloc_chunk_num() != 0)
        {
            // cout << "release by index" << endl;
            int rel_index = rand() % get_mm_alloc_chunk_num();
            // cout << "rel_index: " << rel_index << endl;
            // cout << "addr: " << get_alloc_addr_by_index(rel_index) << endl;
            mm_release(get_alloc_addr_by_index(rel_index));
        }

        // 随机策略 [2]:随机请求多个已分配区块
        if (get_mm_alloc_chunk_num() != 0)
        {
            int rel_times = rand() % get_mm_alloc_chunk_num() + 1;

            for (int i = 0; i < rel_times; ++i)
            {
                if (get_mm_alloc_chunk_num() != 0)
                {
                    int rel_index = rand() % get_mm_alloc_chunk_num();
                    mm_release(get_alloc_addr_by_index(rel_index));
                }
            }
        }
    }

    cout << "avg uziliazation: " << ((int)(sum_mm_utilization * 10000) / steps * 1.0 / 100) << "%" << endl;
    cout << "avg query counts: " << (*query_counts / steps) << endl;
    cout << "request_num: " << request_num << endl;
    // 释放内存
    mm_clear();
}