/**
 * @file struct.h
 * @author xuanrong gu
 * @brief base struct definition
 * @version 0.1
 * @date 2024-12-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

struct Chunk // 内存块结构体 包括空闲分区和已分配内存区
{
    char *addr;  // 块首地址指针
    int size;    // 块大小
    int is_free; // 是否为空闲块
};

struct DubNode // 双向链表节点
{
    void *data;     // 数据域
    DubNode *prior; // 上一个节点指针
    DubNode *next;  // 下一个节点指针
};

struct DubLinkList // 双向链表：采用链表尾单哨兵的模式
{
    DubNode *head; // 头指针
    DubNode *end;  // 尾指针 为哨兵指针，用来迭代使用
    int size;      // 链表节点数
};

struct MemmoryManager // 内存管理对象
{
    DubLinkList *list;

    // 用来计算系统内存利用率：
    int free_m;  // 空闲内存大小(字节数)
    int alloc_m; // 已分配内存大小（字节数）

    // 当前空闲区块和已分配区块个数
    // 为后续随机挑选一个内存分配空间完成释放操作提供模
    int free_chunk_num;
    int alloc_chunk_num;
};

// struct MRequest // 请求结构体
// {
//     int size; // 请求空间大小
// };
