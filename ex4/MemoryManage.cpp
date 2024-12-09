#include <iostream>
#include "struct.h"

using namespace std;

#define MEM_SIZE 1024

char mm[MEM_SIZE];       // 模拟内存
MemmoryManager MManager; // 内存管理对象

void *mm_request(int n);  // 请求 n 个字节的内存空间
void mm_release(void *p); // 释放先前申请的空间
void mm_init();           // 内存初始化。

int main()
{
    mm_init();
    return 0;
}

void *mm_request(int n)
{
}

void mm_release(void *p)
{
}

void mm_init()
{
    // [1] 为 MManager 实例化双向链表对象
    MManager.list = new DubLinkList;

    // [2] 为双向链表建立头指针和尾指针
    // [2.1] 头指针指向初始 mm 数组 这一块完整的空闲区 首地址为 mm,大小为 MM_SIZE
    MManager.list->head = new DubNode;
    MManager.list->end = new DubNode;
    MManager.list->end->data = nullptr;              // 尾哨位指针数据域为空
    MManager.list->end->next = nullptr;              // 尾指针的后继指向空
    MManager.list->end->prior = MManager.list->head; // 初始时尾指针的前驱节点为头指针
    MManager.list->head->prior = nullptr;            // 头指针的前驱节点为空
    MManager.list->head->next = MManager.list->end;  // 初始时头指针的后继节点为尾指针

    Chunk *init_chunk = new Chunk; // 初始内存空闲块
    init_chunk->addr = mm;
    init_chunk->is_free = 1;
    init_chunk->size = MEM_SIZE;

    MManager.list->head->data = init_chunk; // 初始化头指针的数据域
}