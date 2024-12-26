#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int T = 0; // 磁道个数

/**
 * @brief 磁盘调度策略
 */
#define FCFS_METHOD 0 // 先来先服务
#define SCAN_METHOD 1 // 电梯算法（扫描算法）
#define SSTF_METHOD 2 // 最短寻道长度优先

/**
 * @brief 生成磁道调度请求函数
 *
 * @param req_que 请求队列
 * @param size   请求数量
 * @param T      磁道数
 */
void req_generate(int *req_que, int size, int T);

/**
 * @brief 磁盘调度过程
 *
 * @param req_que 请求队列
 * @param size    请求队列大小
 * @param method  使用的调度策略
 * @param avg_arm_distance  平均寻道长度
 */
void schedulue(int *req_que, int size, int method, float *avg_arm_distance);

// 不同策略调度函数
void fcfs_schedule(int *req_que, int size, float *avg_arm_distance);
void scan_schedule(int *req_que, int size, float *avg_arm_distance);
void sstf_schedule(int *req_que, int size, float *avg_arm_distance);

// 输出显示函数
void show_req_que(int *req_que, int size);

// 不同模式对应的程序
void debug();
void experiment();

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("usage:%s mode[0-Debug 1-Experiment] T[Disk Trace Number]\n", argv[0]);
        exit(0);
    }
    srand(time(NULL));        // 生成随机数种子
    int mode = atoi(argv[1]); // 模式
    T = atoi(argv[2]);        // 初始化磁道个数

    printf("|--------------磁盘调度实验-------------|\n");

    switch (mode)
    {
    case 0:
    {
        debug(); // 调试模式
        break;
    }
    case 1:
    {
        experiment(); // 实验模式
        break;
    }
    default:
        break;
    }

    return 0;
}

void req_generate(int *req_que, int size, int T)
{
    // 请求生成策略：随机生成磁道请求（1<=n<=T）
    int n;
    for (int i = 0; i < size; ++i)
    {
        n = rand() % T + 1;
        req_que[i] = n;
    }
}

void schedulue(int *req_que, int size, int method, float *avg_arm_distance)
{

    switch (method)
    {
    case FCFS_METHOD:
    {
        fcfs_schedule(req_que, size, avg_arm_distance);
        break;
    }
    case SCAN_METHOD:
    {
        scan_schedule(req_que, size, avg_arm_distance);
        break;
    }
    case SSTF_METHOD:
    {
        sstf_schedule(req_que, size, avg_arm_distance);
        break;
    }
    default:
        break;
    }
}

void fcfs_schedule(int *req_que, int size, float *avg_arm_distance)
{
    // 初始化磁臂位置（T 代表磁道数）,默认磁臂从磁道中间开始
    *avg_arm_distance = 0;
    int t_point = T / 2;
    int i;

    printf("Disk Trace: %d", t_point);

    for (i = 0; i < size; ++i)
    {
        *avg_arm_distance += abs(t_point - req_que[i]);
        t_point = req_que[i]; // 移动磁臂

        printf(" -> %d", t_point);
    }
    printf("\n");

    *avg_arm_distance /= size;
}
void scan_schedule(int *req_que, int size, float *avg_arm_distance)
{
    // 初始化磁臂位置
    *avg_arm_distance = 0;
    int t_point = T / 2;
    int direction = 1; // 磁臂默认运动方向为磁道递增的方向 (-1-递减方向 | 1-递增方向)

    printf("Disk Trace: %d", t_point);

    while (1)
    {
        int i = 0;
        int index = -1; // 满足条件的请求序号
        int diff = T;   // 满足条件的请求磁道和当前磁臂所在磁道的距离

        // 在当前运动方向上寻找距离当前磁臂距离最近的请求
        for (i = 0; i < size; ++i)
        {
            if (req_que[i] != -1 && (((req_que[i] - t_point) * direction) >= 0) && diff > abs(req_que[i] - t_point))
            {
                diff = abs(req_que[i] - t_point);
                index = i;
            }
        }

        if (index == -1) // 若在当前方向上未找到满足条件得到请求
        {
            if (direction == -1) // 磁臂运动方向若将重新回到初始方向，表示所有请求已满足
            {
                break;
            }
            // 调转方向
            // 当前方向上的请求已全部满足，将磁臂移到该方向上的边界磁道上并改变磁臂运动方向：
            // 递减侧边界 sub_edge = 1 | 递增侧边界 add_edge = T
            int edge = -((direction * (direction - 1)) >> 1) + T * ((direction * (direction + 1)) >> 1); // 找到当前方向上的边界，具体参考 note.md: 关于给出 f(1) = a,f(-1) = b 构造 f(x) 的思考
            *avg_arm_distance += abs(edge - t_point);
            t_point = edge;
            direction = -direction;
            printf(" -> %d", t_point);
            continue;
        }

        // 若找到满足条件的请求：
        *avg_arm_distance += diff;
        t_point = req_que[index];
        req_que[index] = -1; // 请求已满足标记

        printf(" -> %d", t_point);
    }
    printf("\n");

    // 计算磁臂移动的平均距离：
    *avg_arm_distance /= size;
}
void sstf_schedule(int *req_que, int size, float *avg_arm_distance)
{
    // 初始化磁臂位置
    *avg_arm_distance = 0;
    int t_point = T / 2;

    printf("Disk Trace: %d", t_point);

    while (1)
    {
        int i = 0;
        int index = -1; // 满足条件的请求序号
        int diff = T;   // 满足条件的请求磁道和当前磁臂所在磁道的距离

        // 距离当前磁臂距离最近的请求
        for (i = 0; i < size; ++i)
        {
            if (req_que[i] != -1 && diff > abs(req_que[i] - t_point))
            {
                diff = abs(req_que[i] - t_point);
                index = i;
            }
        }

        if (index == -1) // 若未找到满足条件得到请求，则已满足所有请求
        {
            break;
        }
        // 若找到满足条件的请求：
        *avg_arm_distance += diff;
        t_point = req_que[index];
        req_que[index] = -1; // 请求已满足标记

        printf(" -> %d", t_point);
    }
    printf("\n");

    // 计算磁臂移动的平均距离：
    *avg_arm_distance /= size;
}

void show_req_que(int *req_que, int size)
{
    printf("request queue: ");
    for (int i = 0; i < size; ++i)
    {
        printf("%d ", req_que[i]);
    }
    printf("\n");
}

// utills
// 数组拷贝
void cp_int(int *from, int *to, int size);

void debug()
{
    int size = 0;               // 请求个数
    int method = FCFS_METHOD;   // 磁盘调度策略
    float avg_arm_distance = 0; // 满足一轮请求的平均移臂距离
    int *req_que;               // 磁盘请求队列

    while (1)
    {
        // [1] 初始化
        // [1.1] 选择调度策略 method
        printf("Disk Scheduling Method(FCFS-0 | SCAN-1 | SSTF 2): ");
        scanf("%d", &method);
        // [1.2] 设置请求个数 size
        printf("Generating request queue size: ");
        scanf("%d", &size);
        // [1.3] 随机生成 size 个请求序列
        req_que = (int *)malloc(sizeof(int) * size); //  req_que 申请空间
        req_generate(req_que, size, T);
        show_req_que(req_que, size);

        // [2.1] 按调度策略分支处理
        // [2.1.1] case 1: FCFS
        // [2.1.2] case 2: SCAN
        // [2.1.3] case 3: SSTF
        schedulue(req_que, size, method, &avg_arm_distance);

        // [3] 计算并输出结果
        printf("Average Arm Distance: %f\n", avg_arm_distance);
    }

    free(req_que);
    return;
}
void experiment()
{
    // 测试多轮计算每种策略的平均寻道次数
    int i;
    int rounds;
    int size = 0; // 请求个数
    int *req_que; // 磁盘请求队列
    float fcfs_avg, scan_avg, sstf_avg;

    while (1)
    {
        // 输入轮数
        printf("input experiments round: ");
        scanf("%d", &rounds);
        printf("Generating request queue size: ");
        scanf("%d", &size);
        req_que = (int *)malloc(sizeof(int) * size); //  req_que 申请空间

        fcfs_avg = 0;
        scan_avg = 0;
        sstf_avg = 0;
        for (i = 0; i < rounds; ++i)
        {
            float fcfs_avg_r, scan_avg_r, sstf_avg_r; // 每轮各策略的平均寻道距离
            // 随机生成 size 个请求序列
            req_generate(req_que, size, T);
            show_req_que(req_que, size);

            int *temp_que = (int *)malloc(sizeof(int) * size);
            // FCFS 测试
            cp_int(req_que, temp_que, size);
            schedulue(temp_que, size, FCFS_METHOD, &fcfs_avg_r);
            fcfs_avg += fcfs_avg_r;
            // SCAN 测试
            cp_int(req_que, temp_que, size);
            schedulue(temp_que, size, SCAN_METHOD, &scan_avg_r);
            scan_avg += scan_avg_r;
            // SSTF 测试
            cp_int(req_que, temp_que, size);
            schedulue(temp_que, size, SSTF_METHOD, &sstf_avg_r);
            sstf_avg += sstf_avg_r;

            free(temp_que);
        }
        fcfs_avg /= rounds;
        scan_avg /= rounds;
        sstf_avg /= rounds;

        printf("Disk Trace Number T : %d\n", T);
        printf("Request Queue Size: %d\n", size);
        printf("FCFS Average Arm Distance In %d rounds: %f\n", rounds, fcfs_avg);
        printf("SCAN Average Arm Distance In %d rounds: %f\n", rounds, scan_avg);
        printf("SSTF Average Arm Distance In %d rounds: %f\n", rounds, sstf_avg);
    }
    return;
}

void cp_int(int *from, int *to, int size)
{
    int i;
    for (i = 0; i < size; ++i)
    {
        to[i] = from[i];
    }
}