#include <iostream>
#include "Person.h"
using namespace std;

int main(int argc, char *argv[])
{

	Person *aa = Person::getInstance();
//	aa->setName("Hello");
//	qDebug()<<aa->getName();
	Person *bb = Person::getInstance();
//	bb->setName("World");
//	qDebug()<<bb->getName();

	cout<<"aa:"<<aa<<endl;
	cout<<"bb:"<<bb<<endl;
	return 0;
}
