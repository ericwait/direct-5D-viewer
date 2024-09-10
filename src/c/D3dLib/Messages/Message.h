#pragma once

#include "Vec.h"
#include "Renderer.h"

class Message
{
public:
	friend class MessageQueue;

	virtual ~Message(){};

protected:
	Message(){}

	virtual bool process() = 0;
};
