#include "Context.h"
#include "ConcreteStateA.h"

Context::Context()
{
	m_pState = ConcreteStateA::Instance();
}
Context::~Context()
{

}

void Context::changeState(State* st)
{
	m_pState = st;
}

void Context::request()
{
	m_pState->handle(this);
}
