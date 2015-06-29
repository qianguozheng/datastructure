#ifndef _PERSON_H_
#define _PERSON_H_
#include <iostream>
#include <string>

class Person
{
public:
	static Person* getInstance(void)
	{
		if (NULL == instance)
		{
			if (NULL == instance)
			{
				instance = new Person();
			}
		}
		return instance;
	}

	static Person *instance;
	
	//void setName(string tmp){name = tmp;}
	//string  getName(){return name;}
private:
	//string name;
	Person(){};

};

#endif
