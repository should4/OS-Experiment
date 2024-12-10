/**
 * @file MemoryManage.cpp
 * @author xuanrong gu
 * @brief Memory Management
 * @version 0.1
 * @date 2024-12-10
 *
 * @copyright Copyright (c) 2024
 */

#include "MemoryManage.h"
#include <iostream>

using namespace std;

char *mm;                                    // 模拟内存
MemmoryManager MManager;                     // 内存管理对象
int cur_alloc_strategy = FIRST_FIT_STRATEGY; // 内存分配策略: 0-first fit | 1-best fit 默认为首次适应策略

void *mm_request(int n, int *query_conunts)
{
    if (n == 0)
    {
        cout << "request error: cannot request a 0 space" << endl;
        return nullptr;
    }
    void *addr = nullptr;
    switch (cur_alloc_strategy)
    {
    case FIRST_FIT_STRATEGY:
    {
        cout << "Use First Fit Stratage" << endl;
        addr = first_fit(n, query_conunts);
        break;
    }
    case BEST_FIT_STRATEGY:
    {
        addr = best_fit(n, query_conunts);
        break;
    }
    }
    return addr;
}

void mm_release(void *alloc_addr)
{
    if (alloc_addr == nullptr)
    {
        cout << "release error:address is Null" << endl;
        return;
    }

    DubNode *p = MManager.list->head->next; // 迭代指针
    DubNode *rel_node = nullptr;            // 代释放内存节点指针
    Chunk *c_data = nullptr;

    while (p != MManager.list->end)
    {
        c_data = (Chunk *)p->data;
        if (c_data != nullptr && c_data->is_free == 0 && c_data->addr == alloc_addr)
        {
            rel_node = p;
            break;
        }
        p = p->next;
    }

    // 判断是否找到符合条件的代释放内存节点
    if (rel_node == nullptr)
    {
        cout << "release error:No such Allocation Area" << endl;
        return;
    }

    Chunk *rel_data = (Chunk *)rel_node->data;          // 带释放节点数据域
    Chunk *prior_data = (Chunk *)rel_node->prior->data; // 前驱节点数据域
    Chunk *next_data = (Chunk *)rel_node->next->data;   // 后继节点数据域

    cout << "rel_data offset: " << (rel_data->addr - mm) << endl;
    cout << "rel_data size: " << rel_data->size << endl;

    // 记录回收内存的大小，为了后续修改块表 free_m，alloc_m 值
    int recycle_size = rel_data->size;

    // 收回已分配区块的 4 中情况
    // case 1: free->rel_node->free
    // case 2: free->rel_node->alloc
    // case 3: alloc->rel_node->free
    // case 4: alloc->rel_node->alloc
    // 判断该释放内存块前面是否是空闲区块：
    if (prior_data != nullptr && prior_data->is_free == 1)
    {
        // 在前面是空闲区块的前提下判断后面是不是空闲区块：
        if (next_data != nullptr && next_data->is_free == 1)
        {
            // case 1
            // 前后都是空闲区块的情况下，合并前中后三个块为一个空闲区块，策略为保留前一个
            // 区块，释放中后两个区块，因为只需要修改前一个区块的大小即可，首地址不用修改：
            prior_data->size += rel_data->size + next_data->size; // Bugs: prior_data += rel_data->size + next_data->size;

            rel_node->prior->next = rel_node->next->next;
            rel_node->next->next->prior = rel_node->prior;

            cout << "after combinition,prior_data size: " << prior_data->size << endl;

            // 细节待完善：为了缓冲区安全，最好将中和后两个节点指针的前继、后继节点指针置为空，防止后续在新创建一个节点时，会将刚刚free掉的指针重新使用，即虽然我们调用了free函数但进程不会直接将这段内存直接归还，而是作为缓存，当后续再次申请时可以直接使用该段内存
            // 释放 middle,next 节点以及数据域内存
            free(next_data);
            free(rel_node->next);
            free(rel_data);
            free(rel_node);

            // 链表节点数减少 2
            MManager.list->size -= 2;

            // 空闲区块数减1，已分配区块数减1
            MManager.free_chunk_num--;
            MManager.alloc_chunk_num--;
        }
        else
        {
            // case 2
            // 前面是空闲区块后面是已分配区块或者是end哨兵节点，将当前区块和前空闲区块合并为一个空闲区块：
            prior_data->size += rel_data->size;
            rel_node->prior->next = rel_node->next;
            rel_node->next->prior = rel_node->prior;

            // 释放空间
            free(rel_data);
            free(rel_node);
        }
    }
    else
    {
        // 在前面是已分配区块（当前节点是头节点时可以一起处理）的前提下判断后面是不是空闲区块：
        if (next_data != nullptr && next_data->is_free == 1)
        {
            // case 3
            // 前面不为空闲区，后面为空闲区，将当前节点状态设置为空闲区块，并将后面空闲区块合并到当前节点：
            rel_data->is_free = 1;
            rel_data->size += next_data->size;
            DubNode *next_node = rel_node->next; // 被移除节点
            next_node->next->prior = rel_node;
            rel_node->next = next_node->next;

            // 释放内存
            free(next_data);
            free(next_node);

            // 链表节点数减少 1
            // 思考：并未考虑并发安全性问题，并发实现需要加锁
            MManager.list->size--;

            // 已分配区块数减1：
            MManager.alloc_chunk_num--;
        }

        else
        {
            // case 4
            // 前面后面均不为空闲区块（已分配区块节点或者无节点），则将该分配区块直接设置为空闲区块：
            rel_data->is_free = 1;

            // 空闲区块数加1，已分配区块数减1：
            MManager.free_chunk_num++;
            MManager.alloc_chunk_num--;
        }
    }

    // 统一修改块表的空闲内存大小以及已分配内存大小
    MManager.free_m += recycle_size;
    MManager.alloc_m -= recycle_size;
}

void mm_init()
{
    // [0] 初始化原始内存区
    mm = new char[MEM_SIZE]; // (char *)malloc(MEM_SIZE);
    // cout << "mm:" << (void *)mm << endl;

    // [1] 为 MManager 实例化双向链表对象
    MManager.list = new DubLinkList;

    // [2] 初始化使用双哨兵的双向链表
    DubNode *head = new DubNode;
    DubNode *end = new DubNode;
    head->data = nullptr;  // 头哨兵指针数据域为空
    end->data = nullptr;   // 尾哨兵指针数据域为空
    head->prior = nullptr; // 头稍兵指针的前驱节点为空
    end->next = nullptr;   // 尾指针的后继指向空
    end->prior = head;     // 初始时尾指针的前驱节点为头指针
    head->next = end;      // 头哨兵节点的后继节点为尾哨兵节点

    MManager.list->head = head;
    MManager.list->end = end;
    MManager.list->size = 2; // 初始化双向链表参数

    // [3] 初始化原始空闲内存区块
    // 头指针指向初始 mm 数组 这一块完整的空闲区 首地址为 mm,大小为 MM_SIZE
    Chunk *init_chunk = new Chunk; // 初始内存空闲区块
    init_chunk->addr = mm;
    init_chunk->is_free = 1;
    init_chunk->size = MEM_SIZE;
    DubNode *init_node = new DubNode;     // 初始空闲区块节点
    init_node->data = (void *)init_chunk; // 初始化空闲区块节点

    // [4] 将首块空闲区块节点插入双向链表
    MManager.list->head->next = init_node;
    MManager.list->end->prior = init_node;
    init_node->prior = MManager.list->head;
    init_node->next = MManager.list->end;

    MManager.list->size++; // 链表节点数加一

    // [5] 初始化 MManager 对象下关于内存管理各项参数的初值
    MManager.free_m = MEM_SIZE;   // 初始状态，空闲内存等于总内存空间
    MManager.alloc_m = 0;         // 初始状态，已分配内存为 0
    MManager.free_chunk_num = 1;  // 初始状态，空闲区块数为 1
    MManager.alloc_chunk_num = 0; // 初始状态，已分配区块数为 0
}

void *first_fit(int n, int *query_counts)
{
    DubNode *p = MManager.list->head->next;
    DubNode *search_free = nullptr; // 搜索空闲指针
    Chunk *c_data;                  // chunk data 数据域

    while (p != MManager.list->end)
    {
        c_data = ((Chunk *)p->data); // 此处使用强制类型转换，没有进行安全性判断，后续的强制类型转换均为判断 ！
        if (c_data != nullptr && c_data->is_free != 0 && c_data->size >= n)
        {
            search_free = p; // 找到满足条件的空闲区
            break;
        }
        p = p->next;
        (*query_counts)++;
    }

    // 如果不存在满足条件的空闲区，则返回空
    if (search_free == nullptr)
    {
        return nullptr;
    }

    // 记录分配区首地址
    char *alloc_addr = ((Chunk *)search_free->data)->addr;

    // 根据找到的空闲区块，创建分配区块
    alloc_by_search_free(n, search_free);

    return alloc_addr;
}

void *best_fit(int n, int *query_counts)
{
    // 初始化 min_diff:表示请求空间与满足条件空闲区的差值，通过迭代的方式找到最小的值，
    // 即找到所有满足条件的空闲区中大小最小的那一个，也就是Best Fit 空闲区
    int min_diff = MEM_SIZE;

    DubNode *p = MManager.list->head->next;
    DubNode *search_free = nullptr; // 满足条件链表节点指针
    Chunk *c_data = nullptr;

    // 遍历完整链表
    while (p != MManager.list->end)
    {
        c_data = (Chunk *)p->data;
        if (c_data->is_free && c_data->size >= n && (c_data->size - n) < min_diff)
        {
            search_free = p;
            min_diff = c_data->size - n;
        }
        p = p->next;
        (*query_counts)++;
    }

    // 判断是否存在符合条件的空闲区
    if (search_free == nullptr)
    {
        return nullptr;
    }

    // 记录分配区首地址
    char *alloc_addr = ((Chunk *)search_free->data)->addr;

    // 根据找到的空闲区块，创建分配区块
    alloc_by_search_free(n, search_free);

    return alloc_addr;
}

void alloc_by_search_free(int n, DubNode *search_free)
{
    Chunk *c_data;
    // 获取符合条件的空闲区块节点的数据域
    c_data = (Chunk *)search_free->data;

    // 判断空闲区大小是否等于分配大小
    if (c_data->size == n) // 恰好等于分配大小
    {
        // 直接将当前空闲块设置为分配块即可
        c_data->is_free = 0;
        // 空闲区块数减1，已分配区块数加1
        MManager.free_chunk_num--;
        MManager.alloc_chunk_num++;
    }
    else // 空闲区大小大于分配区大小
    {

        // 建立分配内存块
        DubNode *alloc_node = new DubNode; // 新节点
        Chunk *alloc_chunk = new Chunk;    // 新数据域
        alloc_chunk->addr = c_data->addr;
        alloc_chunk->size = n;
        alloc_chunk->is_free = 0;
        alloc_node->data = (void *)alloc_chunk;

        // 修改空闲区参数 收缩空闲区
        c_data->size -= n;
        c_data->addr += n;

        // 插入该分配内存块到链表
        alloc_node->prior = search_free->prior;
        alloc_node->next = search_free;
        search_free->prior->next = alloc_node;
        search_free->prior = alloc_node;

        // 已分配区块数加一
        MManager.alloc_chunk_num++;
        // 链表长度加一
        MManager.list->size++;
    }

    // 空闲内存大小减 n，已分配内存大小加 n:
    MManager.free_m -= n;
    MManager.alloc_m += n;
}

void set_alloc_strat(int strategy)
{
    cur_alloc_strategy = strategy;
}

void *get_addr_by_offset(int offset)
{
    return (void *)(mm + offset);
}

float get_mm_utilization()
{
    return (MManager.alloc_m * 1.0 / MEM_SIZE);
}

void show_mm()
{
    cout << "---------List All Memeory Chunks----------" << endl;
    cout << "list size: " << MManager.list->size << endl;
    cout << "free_chunk_num: " << MManager.free_chunk_num << endl;
    cout << "alloc_chunk_num: " << MManager.alloc_chunk_num << endl;
    cout << "free_m: " << MManager.free_m << endl;
    cout << "alloc_m: " << MManager.alloc_m << endl;
    cout << "Chunks:" << endl;

    int c_index = 0;
    DubNode *p = MManager.list->head->next;
    while (p != MManager.list->end)
    {
        Chunk *c_data = (Chunk *)p->data;
        cout << "|---------------------------------|" << endl;
        cout << "|------------" << "Chunk " << c_index++ << "-------------|" << endl;
        cout << "|  address: " << (void *)c_data->addr << " -> " << (void *)(c_data->addr + c_data->size - 1) << "  |" << endl;
        cout << "|  offset address:     " << (c_data->addr - mm) << "      |" << endl;
        cout << "|  is_free:     " << c_data->is_free << "      |" << endl;
        cout << "|  size:       " << c_data->size << "     |" << endl;
        cout << "|---------------------------------|" << endl;
        p = p->next;
    }
    cout << "------------------------------------------" << endl;
}