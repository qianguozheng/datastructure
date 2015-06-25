#ifndef _CONCRETESTATEB_H_
#define _CONCRETESTATEB_H_

#include "State.h"
class ConcreteStateB: public State
{
public:
	virtual ~ConcreteStateB();
	static State * Instance();
	virtual void handle(Context *c);
private:
	ConcreteStateB();
	static State *m_pState;
};

#endif
