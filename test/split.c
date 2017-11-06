#include <stdio.h>
#include <iostream>

using namespace std;
int main()
{
	char *str="<script src=\"http://www.baidu.com\"></script>";
	char ss[100];
	
	string s;

	memset(ss, 0, sizeof(ss));
	sprintf(ss, "%s", str);
	s = string(ss);

	cout<<s<<endl;
}
