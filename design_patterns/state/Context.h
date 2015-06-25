#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "State.h"

class Context
{
public:
	Context();
	virtual ~Context();
	
	void changeState(State* st);
	void request();

private:
	State *m_pState;
};
#endif
