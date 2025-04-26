#pragma once

#include "Messages/Message.h"

// This message forces a render update
class MessageUpdateRender: public Message
{
public:
	MessageUpdateRender() {}

protected:
	virtual bool process();
};