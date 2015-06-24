#include "context.h"

Context::Context(){
}

Context::~Context(){
}

void Context::algorithm(){
	m_pStrategy->algorithm();
}

void Context::setStrategy(Strategy* st){
	m_pStrategy = st;
}

