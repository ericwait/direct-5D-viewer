#include "ErrorMsg.h"
#include "Globals.h"
#include "MessageQueue.h"

#include <comdef.h>

void errMessage(std::string message, const char *file, int line)
{
	char ln[5];
	sprintf(ln, "%d", line);
	std::string msg = message;
	msg += " In file:";
	msg += file;
	msg += " line ";
	msg += ln;

	gMsgQueueToMex.addErrorMessage(msg);
}

void hrErrMessage(HRESULT hr, const char *file, int line)
{
	_com_error err(hr);
	LPCTSTR errMsg = err.ErrorMessage();

	char buff[128];
	sprintf(buff, "%d", hr);

	std::string msg = errMsg;
	msg += " With val: ";
	msg += buff;
	msg += " In file:";
	msg += file;

	sprintf(buff, "%d", line);
	msg += " line ";
	msg += buff;

	gMsgQueueToMex.addErrorMessage(msg);
}
