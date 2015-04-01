#include <queue>

struct Message
{
	std::string command;
	void *data;
};

class DataQueue
{
public:
	DataQueue();
	~DataQueue();
	Message getNextMessage();
	void addMessage(Message msg);
	void writeMessage(std::string cmd, void* data);
	size_t DataQueue::getNumMessages();


private:
	std::queue<Message> messages;
};