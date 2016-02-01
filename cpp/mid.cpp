/*************************************************************************
	> File Name: mid.cpp
	> Author: qianguozheng
	> Mail: guozhengqian0825@126.com
	> Created Time: Thu Jul 30 18:47:18 2015
 ************************************************************************/

#include<iostream>
using namespace std;

void world()
{
    //printf("hello world\n");
    cout<<"Hello World"<<endl;
}

extern "C" {

    void m_world()
    {
        world();
    }
}
