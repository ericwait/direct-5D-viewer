%% Setup
function varargout = ViewerControls(varargin)
% VIEWERCONTROLS MATLAB code for ViewerControls.fig
%      VIEWERCONTROLS, by itself, creates a new VIEWERCONTROLS or raises the existing
%      singleton*.
%
%      H = VIEWERCONTROLS returns the handle to a new VIEWERCONTROLS or the handle to
%      the existing singleton*.
%
%      VIEWERCONTROLS('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in VIEWERCONTROLS.M with the given input arguments.
%
%      VIEWERCONTROLS('Property','Value',...) creates a new VIEWERCONTROLS or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before ViewerControls_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to ViewerControls_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help ViewerControls

% Last Modified by GUIDE v2.5 10-Jun-2014 19:19:25

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @ViewerControls_OpeningFcn, ...
                   'gui_OutputFcn',  @ViewerControls_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT
end

% --- Executes just before ViewerControls is made visible.
function ViewerControls_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to ViewerControls (see VARARGIN)

% Choose default command line output for ViewerControls
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

setupData(hObject, handles);

% UIWAIT makes ViewerControls wait for user response (see UIRESUME)
% uiwait(handles.figure1);
end

function setupData(hObject, handles)
global imageData channelData

channelData = [];

if isempty(imageData)
    return
end

if (exist([imageData.DatasetName '.mat'],'file'))
    load([imageData.DatasetName '.mat']);
end

if (isempty(channelData))
    
    colors = [[0.0, 1.0, 0.0];...
        [1.0, 0.0, 0.0];...
        [0.0, 0.0, 1.0];...
        [1.0, 1.0, 0.0];...
        [0.0, 1.0, 1.0];...
        [1.0, 0.0, 1.0]];
    
    localStruct = struct(...
        'color',[0,0,0],...
        'visible',1,...
        'alphaMod',1.0,...
        'minVal',0.0,...
        'midVal',0.5,...
        'maxVal',1.0,...
        'a',0.0,...
        'b',1.0,...
        'c',0.0);
    
    channelData = localStruct;
    
    if (imageData.NumberOfChannels>0)
        for i=1:imageData.NumberOfChannels
            localStruct.color = colors(i,:);
            channelData(i) = localStruct;
        end
    end
end

if (imageData.NumberOfChannels>0)
    for i=1:imageData.NumberOfChannels
        if i==1
            strng = sprintf('Channel %d',i);
        else
            strng = {strng; sprintf('Channel %d',i)};
        end
    end
end

set(handles.m_channelPicker,'string',strng);
set(handles.tb_numFrames,'string',num2str(imageData.NumberOfFrames));
set(handles.tb_numChan,'string',num2str(imageData.NumberOfChannels));
set(handles.tb_title,'string',imageData.DatasetName);
set(handles.tb_curFrame,'string',num2str(1));
set(handles.tb_xDim,'string',num2str(imageData.XDimension));
set(handles.tb_yDim,'string',num2str(imageData.YDimension));
set(handles.tb_zDim,'string',num2str(imageData.ZDimension));
set(handles.tb_phyX,'string',num2str(imageData.XPixelPhysicalSize));
set(handles.tb_phyY,'string',num2str(imageData.YPixelPhysicalSize));
set(handles.tb_phyZ,'string',num2str(imageData.ZPixelPhysicalSize));

set(handles.s_curFrame,'max',imageData.NumberOfFrames);
set(handles.s_curFrame,'Value',1);
set(handles.s_curFrame,'min',1);

updateCurrentState(handles);
end

%% Calcualtions
function updateCurrentState(handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
set(handles.s_alpha,'Value',channelData(chan).alphaMod,'BackgroundColor',[channelData(chan).alphaMod/2,channelData(chan).alphaMod/2,channelData(chan).alphaMod/2]);
set(handles.s_floor,'Value',channelData(chan).minVal,'BackgroundColor',channelData(chan).color .* 0.35);
set(handles.s_mids,'Value',channelData(chan).midVal,'BackgroundColor',channelData(chan).color .* 0.75);
set(handles.s_ceil,'Value',channelData(chan).maxVal,'BackgroundColor',channelData(chan).color);
set(handles.pb_colorChooser,'BackgroundColor',channelData(chan).color);
set(handles.cb_visible,'Value',channelData(chan).visible);

plotTransferFunctions(handles)
end

function plotTransferFunctions(handles)
global channelData

cla(handles.funcPlot);
hold on

for i=1:length(channelData)
    [minRange, maxRange] = calcMidRange(channelData(i).minVal,channelData(i).maxVal);
    
    [x, y, channelData(i).a, channelData(i).b, channelData(i).c]...
        = calcCurve(channelData(i).visible, channelData(i).minVal,...
        channelData(i).midVal*(maxRange-minRange)+minRange,...
        channelData(i).maxVal);
    
    if (channelData(i).visible>0)
        plot(handles.funcPlot,x,y,'-','Color',channelData(i).color,'LineWidth',2.0);
    else
        plot(handles.funcPlot,x,y,':','Color',channelData(i).color,'LineWidth',2.0);
    end
end
xlim([0 1]);
ylim([0 1]);

hold off
saveData();
lever_3d('transferUpdate',channelData);
end

function [x, y, a, b, c] = calcCurve(vis,minX, midY, maxX)
x1 = minX;
y1 = 0;
x3 = maxX;
y3 = 1;
x2 = max(0,min(1,(x1+x3)/2));
y2 = midY;

a = ((y2-y1)*(x1-x3) + (y3-y1)*(x2-x1)) / ((x1-x3)*(x2.^2-x1.^2) + (x2-x1)*(x3.^2-x1.^2));
b = ((y2-y1) - a*(x2.^2-x1.^2)) / (x2-x1);
c = y1 - a*x1.^2 - b*x1;
x = minX:0.01:maxX;
y = a*x.^2 + b*x + c;

end

function [ymin, ymax] = calcMidRange(minX, maxX)
x1 = minX;
y1 = 0;
x3 = maxX;
y3 = 1;

x2 = max(0,min(1,(x1+x3)/2));
y2 = 0:0.01:1;

a = ((y2-y1)*(x1-x3) + (y3-y1)*(x2-x1)) ./ ((x1-x3)*(x2.^2-x1.^2) + (x2-x1)*(x3.^2-x1.^2));
b = ((y2-y1) - a*(x2.^2-x1.^2)) ./ (x2-x1);

mm = (2*a*x1+b>=0)&(2*a*x3+b>=0);
ymin = y2(find(mm,1,'first'));
ymax = y2(find(mm,1,'last'));
end

% --- Executes on button press in save.
function saveData()
global imageData channelData

if (~isempty(imageData) && ~isempty(channelData))
    save([imageData.DatasetName '.mat'],'channelData');
end
end

% --- Outputs from this function are returned to the command line.
function varargout = ViewerControls_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles.output;
end

% --- Executes when user attempts to close figure1.
function figure1_CloseRequestFcn(hObject, eventdata, handles)
lever_3d('close');
delete(hObject);
clear mex
end

%% Callback functions

% --- Executes on slider movement.
function s_curFrame_Callback(hObject, eventdata, handles)
frame = round(get(handles.s_curFrame,'Value'));
set(handles.tb_curFrame,'String',num2str(frame));
end

% --- Executes on slider movement.
function s_ceil_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
mxVal = get(handles.s_ceil,'Value');
mnVal = get(handles.s_floor,'Value');

if (mxVal<=mnVal)
    mnVal = mxVal-0.01;
    if (mnVal<0)
        mnVal = 0;
        mxVal = 0.01;
        set(handles.s_ceil,'Value',mxVal);
    end
    set(handles.s_floor,'Value',mnVal);
    channelData(chan).minVal = mnVal;
end
channelData(chan).maxVal = mxVal;

plotTransferFunctions(handles);
end

% --- Executes on slider movement.
function s_mids_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
channelData(chan).midVal = get(handles.s_mids,'Value');

plotTransferFunctions(handles);
end

% --- Executes on slider movement.
function s_floor_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
mxVal = get(handles.s_ceil,'Value');
mnVal = get(handles.s_floor,'Value');

if (mxVal<=mnVal)
    mxVal = mnVal+0.01;
    if (mxVal>1)
        mxVal = 1;
        mnVal = 0.99;
        set(handles.s_floor,'Value',mnVal);
    end
    set(handles.s_ceil,'Value',mxVal);
    channelData(chan).maxVal = mxVal;
end
channelData(chan).minVal = mnVal;

plotTransferFunctions(handles);
end

% --- Executes on slider movement.
function s_alpha_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
channelData(chan).alphaMod = get(handles.s_alpha,'Value');
plotTransferFunctions(handles);
end

% --- Executes on selection change in m_channelPicker.
function m_channelPicker_Callback(hObject, eventdata, handles)
updateCurrentState(handles);
end

% --- Executes on button press in pb_colorChooser.
function pb_colorChooser_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');

channelData(chan).color = uisetcolor(channelData(chan).color);
updateCurrentState(handles);
plotTransferFunctions(handles);
end

% --- Executes on button press in cb_visible.
function cb_visible_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
channelData(chan).visible = get(handles.cb_visible,'Value');

plotTransferFunctions(handles);
end

% --- Executes on slider movement.
function s_peelSize_Callback(hObject, eventdata, handles)

end

% --- Executes on button press in cb_textureLighting.
function cb_textureLighting_Callback(hObject, eventdata, handles)

end


% --- Executes on button press in cb_segLighting.
function cb_segLighting_Callback(hObject, eventdata, handles)

end

%% Create Functions
% --- Executes during object creation, after setting all properties.
function s_curFrame_CreateFcn(hObject, eventdata, handles)
% hObject    handle to s_curFrame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
end

% --- Executes during object creation, after setting all properties.
function s_ceil_CreateFcn(hObject, eventdata, handles)
% hObject    handle to s_ceil (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
end

% --- Executes during object creation, after setting all properties.
function s_mids_CreateFcn(hObject, eventdata, handles)
% hObject    handle to s_mids (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
end

% --- Executes during object creation, after setting all properties.
function s_floor_CreateFcn(hObject, eventdata, handles)
% hObject    handle to s_floor (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
end

% --- Executes during object creation, after setting all properties.
function s_alpha_CreateFcn(hObject, eventdata, handles)
% hObject    handle to s_alpha (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
end

% --- Executes during object creation, after setting all properties.
function m_channelPicker_CreateFcn(hObject, eventdata, handles)
% hObject    handle to m_channelPicker (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end

% --- Executes during object creation, after setting all properties.
function s_peelSize_CreateFcn(hObject, eventdata, handles)
% hObject    handle to s_peelSize (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
end
