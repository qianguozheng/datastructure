#ifndef _STATE_H_
#define _STATE_H_

class Context;
class State{

public:
	virtual void handle(Context *c) = 0;
};

#endif
