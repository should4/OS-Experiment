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

struct MemmoryManager // 内存管理类型的双向链表：继承了基本的双向链表
{
    DubLinkList *list;

    // 用来计算系统内存利用率：
    int free_m;  // 空闲内存大小(字节数)
    int alloc_m; // 已分配内存大小（字节数）
};
