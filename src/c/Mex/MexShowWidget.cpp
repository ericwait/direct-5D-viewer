#include "MexCommand.h"
#include "Global/Globals.h"

#include "Messages/ViewMessages.h"

void MexShowWidget::execute(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	bool widgetOn = (mxGetScalar(prhs[0]) != 0.0);
	gMsgQueueToDirectX.pushMessage(new MessageShowObjectType(GraphicObjectTypes::Widget, widgetOn));
	gMsgQueueToDirectX.pushMessage(new MessageUpdateRender());
}

std::string MexShowWidget::check(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) const
{
	if(nrhs!=1)
		return "Not the right arguments for showWidget!";

	return "";
}

void MexShowWidget::usage(std::vector<std::string>& outArgs, std::vector<std::string>& inArgs) const
{
	inArgs.push_back("on");
}

void MexShowWidget::help(std::vector<std::string>& helpLines) const
{
	helpLines.push_back("This will toggle the Widget on and off.");

	helpLines.push_back("\tOn - If this is set to one, then the Widget will be on. If this is 0, the Widget will be off.");
}