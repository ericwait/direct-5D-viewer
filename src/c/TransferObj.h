#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"

class TransferObj
{
public:
	int chan;
	int fvtIdx;
	Vec<float> transferFunction;
	Vec<float> ranges;
	Vec<float> color;
	float alphaMod;

	TransferObj(int fvtIdx, Vec<float> transferFunction, Vec<float> ranges, Vec<float> color, float alphaMod, int chan);
	~TransferObj();
};