#include <iostream>
#include <vector>

using namespace std;

int main()
{
	vector<string> vec;
	string t = string("qsz.qq.com");
	string t1 = string("magicwifi.com.cn");
	vec.push_back(t);
	vec.push_back(t1);
	
	string cmp = string("http://qsz.qq.com/wx/qq/xxx");
	string cmp1 = string("http://magicwifi.com.cn/device/terminal_mac");
	
	//vector<string>::iterator iter;
	
	for (vector<string>::const_iterator citer = vec.begin(); citer !=vec.end(); citer++)
	{
		cout<<*citer<<endl;
		//int ret = (*citer).find(cmp, 0);//return the location of the found string
		//int ret = cmp.find(*citer, 7);
		int ret = cmp.compare(7, (*citer).length(), (*citer));
		//int ret = cmp.compare(0, cmp.length(), (*citer));
		cout<< "Return Value: "<< ret <<endl;
	}
	
	//Release vector using memory buffer, IMPORTANT
	vec.clear();
	cout<< "Capacity: "<<vec.capacity()<<endl;
	vector<string>(vec).swap(vec);
	cout<< "Capacity: "<<vec.capacity()<<endl;
	
}
