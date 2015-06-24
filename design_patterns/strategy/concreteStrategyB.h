#ifndef _CONCRETESTRATEGY_B_H_
#define _CONCRETESTRATEGY_B_H_

#include "strategy.h"
class ConcreteStrategyB : public Strategy
{
public:
	ConcreteStrategyB();
	virtual ~ConcreteStrategyB();

	virtual void algorithm();
};

#endif
