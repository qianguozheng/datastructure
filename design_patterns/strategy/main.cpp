#include "strategy.h"
#include "context.h"
#include "concreteStrategyA.h"
#include "concreteStrategyB.h"
#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{
	Strategy* s1 = new ConcreteStrategyA();
	Context* cxt = new Context();
	cxt->setStrategy(s1);
	cxt->algorithm();

	Strategy* s2 = new ConcreteStrategyB();
	cxt->setStrategy(s2);
	cxt->algorithm();

	delete s1;
	delete s2;
	return 0;
}
