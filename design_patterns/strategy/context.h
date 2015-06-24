#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "strategy.h"

class Context
{
public:
	Context();
	virtual ~Context();
	
	void algorithm();
	void setStrategy(Strategy* st);

private:
	Strategy* m_pStrategy;
};

#endif
