/**
 * @file deadlock.cpp
 * @author xuanrong gu @3085995438@qq.com
 * @brief
 * @version 0.1
 * @date 2024-12-12
 * @copyright Copyright (c) 2024
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include "utils.h"

using namespace std;
int m = 0;                           // 资源种类
int k = 0;                           // 进程个数
vector<int> Init = {};               //  Init[j] 代表j类资源初始化时的总个数
vector<int> Available = {};          // Available[j] = init[j] -  求和所有（Allocation[0][j]+Allocation[1][j]+…+Allocation[k][j]）（代表j类资源当前状态系统可用个数）
vector<vector<int>> Max = {};        // Max[i][j] i代表进程序号 j代表资源种类 （每个进程对每类资源的最大需求个数）
vector<vector<int>> Allocation = {}; // 每个进程当前占有各类资源个数
vector<vector<int>> Need = {};       // Need[i][j] = Max[i][j] - Allocation[i][j] 表示第i个进程还需要j类资源多少个
vector<int> safe_order = {};         // 一个安全序列

void sysInit();                                             // 系统资源初始化
void showMatrix();                                          // 显示所有矩阵
bool judge();                                               // 使用银行家算法判断系统当前状态是否安全
bool request(const int &p_num, const vector<int> &req_res); // 进程发出请求资源的申请，若系统能立即分配，则返回 1否则返回 0

int main()
{
    sysInit();
    while (true)
    {
        int choose;
        cout << "1 judge" << endl;
        cout << "2 request" << endl;
        cout << "3 showMatrix" << endl;
        cout << "4 exit" << endl;
        cin >> choose;

        switch (choose)
        {
        case 1:
        {
            bool is_safe = judge();
            if (is_safe) // 如果安全输出安全序列
            {
                cout << "当前系统安全,安全序列为 : ";
                for (const auto &safe_p : safe_order)
                {
                    cout << safe_p << " ";
                }
                cout << endl;
            }
            else
            {
                cout << "当前系统不安全，不存在安全序列!" << endl;
            }
            break;
        }
        case 2:
        {
            int p_num; // 请求的进程号

            cout << "进程号:";
            cin >> p_num;
            cin.ignore(5, '\n'); // 清空缓冲区
            string num_line;
            getline(cin, num_line);
            vector<int> req_res(Utils::split(num_line)); // 请求各类资源的个数
            if (p_num < 0 || p_num > k || req_res.size() != m)
            {
                Utils::perror(0, "进程号不合法或者输入资源数不合法", 1);
                continue;
            }
            bool req_ok = request(p_num, req_res);
            if (req_ok)
            {
                cout << "请求成功 !" << endl;
            }
            else
            {
                cout << "请求失败 !" << endl;
            }
            break;
        }
        case 3:
        {
            showMatrix();
            break;
        }
        case 4:
        {
            exit(0);
            break;
        }
        default:
            break;
        }
    }

    return 0;
}

void sysInit()
{
    cout << "输入资源种类数 m: ";
    cin >> m;
    cout << "输入进程个数 k: ";
    cin >> k;

    string num_line; // 存储一行输入的数字
    cout << "请在一行中按顺序输入每个资源的个数: ";
    cin.ignore(10, '\n'); // 清空缓冲区
    getline(cin, num_line);
    Init = Utils::split(num_line);
    Utils::perror(Init.size() == m, "资源种类数错误");

    // 输入每个进程的最大资源获取数 以及当前已经获取的资源数
    Max.resize(k);
    Allocation.resize(k);
    Need.resize(k);
    cout << "请依次输入每个进程的最大资源获取数，以及当前已经获取的资源数" << endl;
    for (int i = 0; i < k; ++i)
    {
        Max[i].resize(m);
        Allocation[i].resize(m);
        Need[i].resize(m);
        for (int j = 0; j < m; ++j)
        {
            int max_j, alloc_j, need_j;
            cout << "Max|Allocation [" << i << "]" << "[" << j << "]: ";
            cin >> max_j >> alloc_j;
            Max[i][j] = max_j;
            Allocation[i][j] = alloc_j;
            Need[i][j] = Max[i][j] - Allocation[i][j];
        }
    }

    // 计算 Avaliable 矩阵
    Available.resize(m);
    for (int j = 0; j < m; ++j)
    {
        int sum_alloc = 0;
        for (int i = 0; i < k; ++i)
        {
            sum_alloc += Allocation[i][j];
        }
        Available[j] = Init[j] - sum_alloc;
        Utils::perror(Available[j] >= 0, "System Available source error"); // 若剩余矩阵中数小于0，则输入非法
    }
}

void showMatrix()
{
    // Allocation ： 分配矩阵
    cout << "-------Allocation--------" << endl;
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            cout << Allocation[i][j] << " ";
        }
        cout << endl;
    }
    cout << "-------------------------" << endl;

    // Max : 最大需求矩阵
    cout << "----------Max------------" << endl;
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            cout << Max[i][j] << " ";
        }
        cout << endl;
    }
    cout << "-------------------------" << endl;

    // Available ： 可用资源矩阵
    cout << "--------Available--------" << endl;
    for (int j = 0; j < m; ++j)
    {
        cout << Available[j] << " ";
    }
    cout << endl;
    cout << "-------------------------" << endl;

    // Need : 需求矩阵
    cout << "----------Need-----------" << endl;
    for (int i = 0; i < k; ++i)
    {
        for (int j = 0; j < m; ++j)
        {
            cout << Need[i][j] << " ";
        }
        cout << endl;
    }
    cout << "-------------------------" << endl;
}

bool judge()
{
    // 初始化 Work-工作向量 Finish-进程状态向量
    vector<int> Work(Available);
    vector<int> Finish;
    Finish.resize(k);
    for (int i = 0; i < k; ++i)
    {
        Finish[i] = 0; // 初始状态下所有进程都处于未完成状态
    }
    safe_order.clear(); // 清空安全序列数组
    // 银行家算法迭代过程
    while (true)
    {
        // 遍历 Need 矩阵，查看是否有进程对所有需求资源满足以下条件
        // Need[i][j] <= Work[j] && Finish[i] = False
        int find_p = -1; // 该轮满足条件的进程号
        for (int i = 0; i < k; ++i)
        {
            if (Finish[i] == 0) // 从未结束的进程中寻找
            {
                int is_fit = 1; // 是否所有需求资源都小于系统当前拥有资源的标识
                for (int j = 0; j < m; ++j)
                {
                    if (Need[i][j] > Work[j])
                    {
                        is_fit = 0;
                        break;
                    }
                }

                if (is_fit == 1) // 如果满足条件
                {
                    for (int j = 0; j < m; ++j)
                    {
                        Work[j] += Allocation[i][j];
                    }
                    Finish[i] = 1; // 将该进程状态标识为结束
                    find_p = i;    // 记录安全序列
                    break;
                }
            }
        }

        if (find_p < 0) // 表示未找到满足条件的进程，退出迭代过程
        {
            break;
        }
        else
        {
            // 将该轮找到满足条件的进程添加到安全序列中
            safe_order.emplace_back(find_p);
        }
    }

    // 判断是否所有进程都处于完成状态
    int all_finished = 1;
    for (int i = 0; i < k; ++i)
    {
        if (Finish[i] != 1) // 若存在未完成的进程
        {
            all_finished = 0;
        }
    }

    return all_finished;
}

bool request(const int &p_num, const vector<int> &req_res)
{
    // [1] 请求的资源数 <= Need
    for (int j = 0; j < m; ++j)
    {
        if (req_res[j] > Need[p_num][j])
        {
            cout << "请求资源数大于需要个数 !" << endl;
            return 0;
        }
    }

    // [2] 请求资源数 <= Available
    for (int j = 0; j < m; ++j)
    {
        if (req_res[j] > Available[j])
        {
            cout << "请求资源数大于系统当前拥有资源数 !" << endl;
            return 0;
        }
    }
    // [3] 尝试分配资源给该进程并判断分配之后系统是否安全
    cout << "尝试分配资源..." << endl;
    for (int j = 0; j < k; ++j)
    {
        Allocation[p_num][j] += req_res[j];
        Need[p_num][j] -= req_res[j];
        Available[j] -= req_res[j];
    }

    bool is_safe = judge();
    if (is_safe) // 如果安全输出安全序列
    {
        cout << "请求允许，分配之后系统仍安全" << endl;
        cout << "分配成功!" << endl;
        cout << "当前系统安全,安全序列为 : ";
        for (const auto &safe_p : safe_order)
        {
            cout << safe_p << " ";
        }
        cout << endl;
    }
    else
    {
        cout << "请求不允许，分配之后系统处于不安全状态" << endl;
        // 撤回分配
        for (int j = 0; j < k; ++j)
        {
            Allocation[p_num][j] -= req_res[j];
            Need[p_num][j] += req_res[j];
            Available[j] += req_res[j];
        }
        cout << "撤回分配" << endl;
    }

    return is_safe;
}