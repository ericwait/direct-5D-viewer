#pragma once

#include "Global/Vec.h"
#include "D3d/Renderer.h"

class Message
{
public:
	friend class MessageQueue;

	virtual ~Message(){};

protected:
	Message(){}

	virtual bool process() = 0;
};
