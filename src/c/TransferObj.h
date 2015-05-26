#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"

class TransferObj
{
public:
	int chan;
	size_t numElem;
	Vec<float> transferFunction;
	Vec<float> ranges;
	Vec<float> color;
	float alphaMod;
	char buff[96];

	TransferObj(Vec<float> transferFunction, Vec<float> ranges, Vec<float> color, float alphaMod, int chan, char* b, size_t numElem);
	~TransferObj();
};