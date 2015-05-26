#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"

class TextureLightingObj
{
public:
	bool value;
	int index;

	TextureLightingObj(bool value, int index);
	~TextureLightingObj();
};