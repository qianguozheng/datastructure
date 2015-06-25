#include "ConcreteStateA.h"
#include "ConcreteStateB.h"
#include "Context.h"
#include <iostream>

using namespace std;

State * ConcreteStateB::m_pState = NULL;
ConcreteStateB::ConcreteStateB(){
}

ConcreteStateB::~ConcreteStateB(){
}

State * ConcreteStateB::Instance()
{
	if (NULL == m_pState)
	{
		m_pState = new ConcreteStateB();
	}
	return m_pState;
}

void ConcreteStateB::handle(Context *c)
{
	cout<<"doing something in State B.\n done, change state to A"<<endl;
	c->changeState(ConcreteStateA::Instance());
}
