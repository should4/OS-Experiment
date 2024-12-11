#pragma once
#include "struct.h"

// 初始内存区大小
#define MEM_SIZE 1024

/**
 * @brief Memory Allocation stratagy
 *
 */
#define FIRST_FIT_STRATEGY 0
#define BEST_FIT_STRATEGY 1

/**
 * @brief 申请n个字节的内存空间。如申请成功，则返回所分配空间的首地址；如不能满足申请，则返回空值。
 *
 * @param n
 * @return void*
 */
void *mm_request(int n, int *query_counts); // 请求 n 个字节的内存空间，query_counts 作为分配性能评判的参数
/**
 * @brief 释放先前申请的内存。如果释放的内存与空闲区相邻，则合并为一个大空闲区；如果与空闲区不相邻，则成为一个单独的空闲区
 *
 * @param p
 */
void mm_release(void *alloc_addr); // 释放先前申请的空间
void mm_init();                    // 内存初始化。
void mm_clear();                   // 内存清除

void dub_linklist_clear(DubLinkList *list); // 双向链表清理

void *first_fit(int n, int *query_counts);
void *best_fit(int n, int *query_counts);
void alloc_by_search_free(int n, DubNode *search_free); // 通过找到满足条件的空闲区来分配

void set_alloc_strat(int strategy);   // 供操作系统调用，用来设置内存分配策略
void *get_addr_by_offset(int offset); // 根据内存块首地址的距离基地址的偏移量获取真实地址

float get_mm_utilization();               // 获取当前内存利用率
int get_mm_alloc_chunk_num();             // 获取当前已分配区块数
void *get_alloc_addr_by_index(int index); // 通过已分配区块序号获取区块首地址

void show_mm(); // 展示内存区块的所有信息
