// proccreate项目
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <tchar.h> // 包含 TCHAR 的定义

//
void StartClone(int nCloneID)
{
    std::cout << "start clone" << nCloneID << std::endl;

    //  提取当前可执行文件的文件名
    TCHAR szFilename[MAX_PATH] ;
    :: GetModuleFileName(NULL, szFilename, MAX_PATH) ;
    // TCHAR szFilename[MAX_PATH] = _T("1-1-child.exe");

    // 格式化用于子进程的命令行，字符串“child”将作为形参传递给子进程的 main 函数
    TCHAR szCmdLine[MAX_PATH];
	:: sprintf(szCmdLine,"\"%s\" %d",szFilename,nCloneID);
    //将下句中的字符串 child 改为别的字符串，重新编译执行，执行前请先保存已经完成的工作
	// 子进程的启动信息结构
    STARTUPINFO si;
    :: ZeroMemory(reinterpret_cast <void*> (&si) , sizeof(si) ) ;
    si.cb = sizeof(si) ;				// 必须是本结构的大小

    // 返回的用于子进程的进程信息
    PROCESS_INFORMATION pi;

    // 利用同样的可执行文件和命令行创建进程，并赋于其子进程的性质
    BOOL bCreateOK=::CreateProcess(
        szFilename,					// 产生这个EXE的应用程序的名称
        szCmdLine,					// 告诉其行为像一个子进程的标志
        NULL,						// 缺省的进程安全性
        NULL,						// 缺省的线程安全性
        FALSE,						// 不继承句柄
        CREATE_NEW_CONSOLE,			// 使用新的控制台
        NULL,						// 新的环境
        NULL,						// 当前目录
        &si,						// 启动信息
        &pi) ;						// 返回的进程信息

    // 释放指向子进程的引用
    if (bCreateOK)
    {
        :: CloseHandle(pi.hProcess) ;//结束进程
        :: CloseHandle(pi.hThread) ;//
    }
    //CloseHandle();之后，切断了子进程和父进程之间的关系，子进程可以独立的运行。
    //TerminateProcess(hProcess) 强制性的终结一个进程。最好不用。
}

int main(int argc, char* argv[] )
{
    //
    int nClone(0);//定义nClone并初始化为0
    if (argc > 1)
    {
        //
        :: sscanf(argv[1] , "%d" , &nClone) ;
    }
    //arc 用来表示在命令行下输入命令的时候，一共有多少个参数。比方说程序编译后，可执行文件是test.exe
    //D:\tc2>test
    //此时argc的值是1
    //D:\tc2>test.exe myarg1 myarg2
    //此时argc的值是3。也就是 命令名 加上两个参数，一共三个参数
    //char *argv[] 用来取得所输入的参数
    //D:\tc2>test
    //此时argc的值是1，argv[0]的值是 “test”
    //D:\tc2>test myarg1 myarg2
    //此时argc的值是3，argc[0]的值是”test”，argc[1]的值是”myarg1”，argc[2]的值是”myarg2”

    // 显示进程位置
    std :: cout << "Process ID:" << :: GetCurrentProcessId()
                << ", Clone ID:" << nClone
                << std :: endl;

    // 定义子进程的最大个数
    const int c_nCloneMax=25;
    if (nClone < c_nCloneMax)
    {
        // 创建子进程
        StartClone(++nClone) ;
    }

    // StartClone(++nClone) ;
    // 在终止之前暂停一下 (l/2秒)
    :: Sleep(500) ;

    return 0;
}
