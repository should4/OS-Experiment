#include <windows.h>
#include <iostream>
#include <stdio.h>

int main(int argc, char* argv[] )
{
    //
    int nChild(0);//定义nClone并初始化为0
    if (argc > 1)
    {
        :: sscanf(argv[1] , "%d" , &nChild) ;
    }

    ::Sleep(3000);
    std::cout << "this is a child process" << std::endl; 
    // 显示进程位置
    std :: cout << "Child Process ID:" << :: GetCurrentProcessId()
                << ", child ID:" << nChild
                << std :: endl;



    // 在终止之前暂停一下 (l/2秒)
    :: Sleep(1000) ;

    return 0;
}