#ifndef _CONCRETESTRATEGY_H_
#define _CONCRETESTRATEGY_H_

#include "strategy.h"
class ConcreteStrategyA : public Strategy
{
public:
	ConcreteStrategyA();
	virtual ~ConcreteStrategyA();

	virtual void algorithm();
};

#endif
