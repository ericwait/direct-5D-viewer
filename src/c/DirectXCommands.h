#include <concrt.h>
#include "Globals.h"
#include "mex.h"
#include "MessageProcessor.h"
#include "MexFunctions.h"
#include "Image.h"

void XloadTextureCommand(Message m);
void XresetViewCommand(Message m);
void XtransferUpdateCommand(Message m);
void XaddHullCommand(Message m);
void XpeelUpdateCommand(Message m);
void XremoveHullCommand(Message m);
void XplayCommand(Message m);
void XrotateCommand(Message m);
void XshowLabelsCommand(Message m);
void XcaptureSpinMovieCommand(Message m);
void XviewTextureCommand(Message m);
void XviewSegmentationCommand(Message m);
void XwireframeSegmentationCommand(Message m);
void XsetFrameCommand(Message m);
void XsetViewOriginCommand(Message m);
void XtextureLightingUpdateCommand(Message m);
void XtextureAttenUpdateCommand(Message m);
void XsegmentationLighting(Message m);
void XdisplayHullsCommand(Message m);
void XdeleteAllHullsCommand(Message m);
