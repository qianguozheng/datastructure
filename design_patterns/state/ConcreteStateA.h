#ifndef _CONCRETESTATEA_H_
#define _CONCRETESTATEA_H_

#include "State.h"
class ConcreteStateA: public State
{
public:
	virtual ~ConcreteStateA();
	static State * Instance();
	virtual void handle(Context *c);
private:
	ConcreteStateA();
	static State *m_pState;
};

#endif
