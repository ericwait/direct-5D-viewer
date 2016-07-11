#undef DEF_MEX_COMMAND
#undef BEGIN_MEX_COMMANDS
#undef END_MEX_COMMANDS

#if defined(INSTANCE_COMMANDS)
#define BEGIN_MEX_COMMANDS
#define END_MEX_COMMANDS
#define DEF_MEX_COMMAND(name) Mex##name _ginstMex##name;
#elif defined(BUILD_COMMANDS)
#define BEGIN_MEX_COMMANDS																								\
	MexCommand* const MexCommand::m_commands[] =												\
							{

#define END_MEX_COMMANDS																								\
							};																							\
							const size_t MexCommand::m_numCommands = sizeof(MexCommand::m_commands) / sizeof(MexCommand*);

#define DEF_MEX_COMMAND(name) &_ginstMex##name,
#else
#define BEGIN_MEX_COMMANDS
#define END_MEX_COMMANDS
#define DEF_MEX_COMMAND(name)																	\
class Mex##name : public MexCommand															\
	{																									\
	public:																								\
	virtual std::string check(int nlhs,mxArray* plhs[],int nrhs,const mxArray* prhs[]) const;	\
	virtual void execute(int nlhs,mxArray* plhs[],int nrhs,const mxArray* prhs[])  const;		\
	\
	virtual void usage(std::vector<std::string>& outArgs,std::vector<std::string>& inArgs)  const;	\
	virtual void help(std::vector<std::string>& helpLines) const;									\
	\
	Mex##name() :MexCommand(#name) {};															\
	};
#endif

BEGIN_MEX_COMMANDS
// These are default commands defined for all MEX routines.
DEF_MEX_COMMAND(Info)
DEF_MEX_COMMAND(Help)
// Additional specific mex commands should be added here.
DEF_MEX_COMMAND(AddPolygons)
DEF_MEX_COMMAND(CaptureSpinMovie)
DEF_MEX_COMMAND(CaptureWindow)
DEF_MEX_COMMAND(Close)
DEF_MEX_COMMAND(DeleteAllPolygons)
DEF_MEX_COMMAND(DisplayPolygons)
DEF_MEX_COMMAND(Init)
DEF_MEX_COMMAND(LoadTexture)
DEF_MEX_COMMAND(PeelUpdate)
DEF_MEX_COMMAND(Play)
DEF_MEX_COMMAND(Poll)
DEF_MEX_COMMAND(ReleaseControl)
DEF_MEX_COMMAND(RemovePolygon)
DEF_MEX_COMMAND(ResetView)
DEF_MEX_COMMAND(Rotate)
DEF_MEX_COMMAND(PolygonLighting)
DEF_MEX_COMMAND(SetBackgroundColor)
DEF_MEX_COMMAND(SetCapturePath)
DEF_MEX_COMMAND(SetFrame)
DEF_MEX_COMMAND(SetRotation)
DEF_MEX_COMMAND(SetViewOrigin)
DEF_MEX_COMMAND(SetWindowSize)
DEF_MEX_COMMAND(ShowLabels)
DEF_MEX_COMMAND(ShowPolygons)
DEF_MEX_COMMAND(ShowTexture)
DEF_MEX_COMMAND(TakeControl)
DEF_MEX_COMMAND(TextureAttenuation)
DEF_MEX_COMMAND(TextureLighting)
DEF_MEX_COMMAND(TransferFunction)
DEF_MEX_COMMAND(ToggleWireframe)
END_MEX_COMMANDS