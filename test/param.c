#include <iostream>
#include <regex>
#include <string>
using namespace std;

class WebUrl {
    public:
        WebUrl(const string& url) : _url(url) {}
        WebUrl(string& url) : _url(move(url)) {}

        string Request(const string& request) const;
    private:
        string _url;
    };
    
string WebUrl::Request(const string& request) const {
	smatch result;
	if (regex_search(_url.cbegin(), _url.cend(), result, regex(request + "=(.*?)&"))) {
		// 匹配具有多个参数的url

		// *? 重复任意次，但尽可能少重复  
		return result[1];
	} else if (regex_search(_url.cbegin(), _url.cend(), result, regex(request + "=(.*)"))) {
		// 匹配只有一个参数的url

		return result[1];
	} else {
		// 不含参数或制定参数不存在

		return string();
	}
}


int main() {
    try 
    {
        WebUrl web("www.123.com/index.aspx?catalog=sport&id=10&rank=20&hello=hello");
        cout << web.Request("catalog") << endl;
        cout << web.Request("id") << endl;
        cout << web.Request("rank") << endl;
        cout << web.Request("hello") << endl;
        cout << web.Request("world") << endl;
    } catch (const regex_error& e) 
    {
        cout << e.code() << endl;
        cout << e.what() << endl;
    }

    return 0;
}
