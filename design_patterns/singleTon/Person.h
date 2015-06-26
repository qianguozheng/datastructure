#ifndef _PERSON_H_
#define _PERSON_H_
#include <iostream>
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
private:
	Person(){};

};

#endif
