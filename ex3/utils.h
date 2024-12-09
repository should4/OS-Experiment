#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;

class Utils
{
public:
    static vector<int> split(string s);                         // 字符串按空格分割
    static int str2int(string str);                             // 字符串转数字
    static bool isInt(const string &str);                       // 判断输入字符串是否是数字
    static void perror(int flag, string errmsg, int level = 0); // 判断错误并输出错误信息
};

bool Utils::isInt(const string &str)
{
    bool result = true;
    // 遍历 str 每个字符，判断其 ASCII 码是否在 0-9 之间
    for (int i = 0; i < str.size(); ++i)
    {
        int num = str.at(i) - '0';
        if (num < 0 || num > 9)
        {
            result = false;
            break;
        }
    }
    return result;
}

vector<int> Utils::split(string s)
{
    s += " ";
    vector<int> split_s;
    string::size_type l = 0, r;
    while (l < s.size())
    {
        if (s.find(" ", l) == string::npos)
        {
            break;
        }
        else if (s.find(" ", l) == l)
        {
            l++;
            continue;
        }

        r = s.find(" ", l);
        // 判断字符串是否可以转为数字
        if (isInt(s.substr(l, r - l)))
        {
            split_s.emplace_back(str2int(s.substr(l, r - l)));
        }
        l = r + 1;
    }
    return split_s;
}

int Utils::str2int(string str)
{
    stringstream ss;
    ss << str;
    int num;
    ss >> num;
    return num;
}

void Utils::perror(int flag, string errmsg, int level)
{
    if (flag == 0)
    {
        cout << "perror:" << errmsg << endl;
        switch (level)
        {
        case 0:
        {

            exit(1);
            break;
        }
        case 1:
        {
            break;
        }

        default:
            break;
        }
    }
}