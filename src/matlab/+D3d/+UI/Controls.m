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

% Last Modified by GUIDE v2.5 18-Feb-2017 10:25:29

% Begin initialization code - DO NOT EDIT

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%Copyright 2014 Andrew Cohen, Eric Wait, and Mark Winter
%This file is part of LEVER 3-D - the tool for 5-D stem cell segmentation,
%tracking, and lineaging. See http://bioimage.coe.drexel.edu 'software' section
%for details. LEVER 3-D is free software: you can redistribute it and/or modify
%it under the terms of the GNU General Public License as published by the Free
%Software Foundation, either version 3 of the License, or (at your option) any
%later version.
%LEVER 3-D is distributed in the hope that it will be useful, but WITHOUT ANY
%WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
%A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
%You should have received a copy of the GNU General Public License along with
%LEVer in file "gnu gpl v3.txt".  If not, see  <http://www.gnu.org/licenses/>.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% Setup
function varargout = Controls(varargin)
gui_Singleton = 1;
gui_State = struct('gui_Name',       fullfile('+D3d','+UI',mfilename), ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @Controls_OpeningFcn, ...
                   'gui_OutputFcn',  @Controls_OutputFcn, ...
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
function Controls_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to ViewerControls (see VARARGIN)

% Choose default command line output for ViewerControls
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

setupData(hObject, handles, varargin);

% UIWAIT makes ViewerControls wait for user response (see UIRESUME)
% uiwait(handles.Controls);
end

function setupData(hObject, handles, varargin)
global D3dUICtrlHandles D3dIsOpen
if (nargin<3)
    error('No image metadata sent!');
end

D3dUICtrlHandles = struct('handles',handles,'hObject',hObject);

imageData = varargin{1}{1};

savedData = [];

if (exist([imageData.DatasetName '_Settings.mat'],'file'))
    savedData = load([imageData.DatasetName '_Settings.mat']);
end

if (isfield(imageData,'ChannelColors') && ~isempty(imageData.ChannelColors))
    colors = imageData.ChannelColors;
else
    colors = [[0.0, 1.0, 0.0];...
        [1.0, 0.0, 0.0];...
        [0.0, 0.0, 1.0];...
        [1.0, 1.0, 0.0];...
        [0.0, 1.0, 1.0];...
        [1.0, 0.0, 1.0]];
end

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

if (~isfield(savedData,'channelData') || isempty(savedData.channelData))
    channelData = localStruct;
    for i=1:imageData.NumberOfChannels
        if (imageData.NumberOfChannels==1)
            localStruct.color = [1,1,1];
            colors(1,:) = [1,1,1];
        else
            localStruct.color = colors(i,:);
        end
        channelData(i) = localStruct;
    end
else
    channelData = savedData.channelData;
    colors = savedData.colors;
end

if (imageData.NumberOfChannels>0)
    if (isfield(imageData,'ChannelColor') && ~isempty(imageData.ChannelColor))
        channelData.color = imageData.ChannelColor;
    end
    if (isfield(imageData,'ChannelNames') && ~isempty(imageData.ChannelNames))
        strng = imageData.ChannelNames;
    else
        for i=1:imageData.NumberOfChannels
            if i==1
                strng = {sprintf('Channel %d',i)};
            else
                strng = [strng; {sprintf('Channel %d',i)}];
            end
        end
    end
end

D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);

set(handles.m_channelPicker,'string',strng);
set(handles.tb_numFrames,'string',num2str(imageData.NumberOfFrames));
if (imageData.NumberOfFrames > 1)
    set(handles.s_curFrame,'Max',imageData.NumberOfFrames,'Min',1,'Enable','on','SliderStep',[1/imageData.NumberOfFrames, 0.1],'Value',1);
else
    set(handles.s_curFrame,'enable','off');
end
set(handles.tb_numChan,'string',num2str(imageData.NumberOfChannels));
set(handles.tb_title,'string',imageData.DatasetName);
set(handles.tb_curFrame,'string',num2str(1));
set(handles.tb_xDim,'string',num2str(imageData.Dimensions(1)));
set(handles.tb_yDim,'string',num2str(imageData.Dimensions(2)));
set(handles.tb_zDim,'string',num2str(imageData.Dimensions(3)));
set(handles.tb_phyX,'string',num2str(imageData.PixelPhysicalSize(1)));
set(handles.tb_phyY,'string',num2str(imageData.PixelPhysicalSize(2)));
set(handles.tb_phyZ,'string',num2str(imageData.PixelPhysicalSize(3)));

D3d.UI.Ctrl.UpdateCurrentState();
D3dIsOpen = true;
end


% --- Executes when user attempts to close Controls.
function Controls_CloseRequestFcn(hObject, eventdata, handles)
global EXT_MESAGE_FUNC D3dIsOpen
if (~isempty(EXT_MESAGE_FUNC))
    msg.command = 'close';
    EXT_MESAGE_FUNC(msg);
end
D3d.Close();
delete(hObject);
D3dIsOpen = false;
end

% --- Outputs from this function are returned to the command line.
function varargout = Controls_OutputFcn(hObject, eventdata, handles)
varargout{1} = handles.output;
end

%% Callback functions

% --- Executes on slider movement.
function s_curFrame_Callback(hObject, eventdata, handles)
frame = round(get(handles.s_curFrame,'Value'));
D3d.UI.Ctrl.UpdateTime(frame);
D3d.Update();
end

% --- Executes on slider movement.
function s_ceil_Callback(hObject, eventdata, handles)
[imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();
chan = get(handles.m_channelPicker,'Value');
mxVal = get(handles.s_ceil,'Value');
mnVal = get(handles.s_floor,'Value');

if (mxVal<0.01)
    mxVal = 0.01;
end

if (mxVal<=mnVal+0.01)
    mnVal = mxVal-0.015;
    if (mnVal<0)
        mnVal = 0;
        mxVal = 0.01;
        set(handles.s_ceil,'Value',mxVal);
    end
    set(handles.s_floor,'Value',mnVal);
    channelData(chan).minVal = mnVal;
end
channelData(chan).maxVal = mxVal;

D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);
D3d.UI.Ctrl.PlotTransferFunctions();
end

% --- Executes on slider movement.
function s_mids_Callback(hObject, eventdata, handles)
[imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();

chan = get(handles.m_channelPicker,'Value');
channelData(chan).midVal = get(handles.s_mids,'Value');

D3d.UI.Ctrl.SetUserData(imageData,colors,channelData)
D3d.UI.Ctrl.PlotTransferFunctions();
end

% --- Executes on slider movement.
function s_floor_Callback(hObject, eventdata, handles)
[imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();

chan = get(handles.m_channelPicker,'Value');
mxVal = get(handles.s_ceil,'Value');
mnVal = get(handles.s_floor,'Value');

if (mnVal>0.99)
    mnVal = 0.99;
end

if (mxVal<=mnVal+0.01)
    mxVal = mnVal+0.015;
    if (mxVal>1)
        mxVal = 1;
        mnVal = 0.99;
        set(handles.s_floor,'Value',mnVal);
    end
    set(handles.s_ceil,'Value',mxVal);
    channelData(chan).maxVal = mxVal;
end
channelData(chan).minVal = mnVal;

D3d.UI.Ctrl.SetUserData(imageData,colors,channelData)
D3d.UI.Ctrl.PlotTransferFunctions();
end

% --- Executes on slider movement.
function s_alpha_Callback(hObject, eventdata, handles)
[imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();

chan = get(handles.m_channelPicker,'Value');
channelData(chan).alphaMod = get(handles.s_alpha,'Value');

D3d.UI.Ctrl.SetUserData(imageData,colors,channelData)
D3d.UI.Ctrl.PlotTransferFunctions();
end

% --- Executes on selection change in m_channelPicker.
function m_channelPicker_Callback(hObject, eventdata, handles)
D3d.UI.Ctrl.UpdateCurrentState();
end

% --- Executes on button press in pb_colorChooser.
function pb_colorChooser_Callback(hObject, eventdata, handles)
[imageData, colors, channelData] = D3d.UI.Ctrl.GetUserData();

chan = get(handles.m_channelPicker,'Value');
channelData(chan).color = uisetcolor(channelData(chan).color);
colors(chan,:) = channelData(chan).color;

D3d.UI.Ctrl.SetUserData(imageData,colors,channelData);
D3d.UI.Ctrl.UpdateCurrentState();
D3d.UI.Ctrl.PlotTransferFunctions();
end

% --- Executes on button press in cb_visible.
function cb_visible_Callback(hObject, eventdata, handles)
    D3d.UI.ToggleChannel(get(handles.m_channelPicker,'Value'),get(handles.cb_visible,'Value'));
end

% --- Executes on button press in cb_textureLighting.
function cb_textureLighting_Callback(hObject, eventdata, handles)
D3d.Viewer.TextureLighting(get(handles.cb_textureLighting,'Value'));
D3d.Update();
end

% --- Executes on button press in cb_atten.
function cb_atten_Callback(hObject, eventdata, handles)
D3d.Viewer.TextureAttenuation(get(handles.cb_atten,'Value'));
D3d.Update();
end

% --- Executes on button press in rb_orgImage.
function rb_orgImage_Callback(hObject, eventdata, handles)
D3d.UI.Ctrl.EnableBuffer(1);
D3d.Update();
end

% --- Executes on button press in rb_Processed.
function rb_Processed_Callback(hObject, eventdata, handles)
D3d.UI.Ctrl.EnableBuffer(2);
D3d.Update();
end

% --- Executes on button press in cb_Wireframe.
function cb_Wireframe_Callback(hObject, eventdata, handles)
on = get(handles.cb_Wireframe,'Value');
D3d.Viewer.ToggleWireframe(on);
D3d.Update();
end

% --- Executes on button press in cb_segLighting.
function cb_segLighting_Callback(hObject, eventdata, handles)
on = get(handles.cb_segLighting,'Value');
D3d.Viewer.PolygonLighting(on);
D3d.Update();
end

% --- Executes on button press in cb_Play.
function cb_Play_Callback(hObject, eventdata, handles)
on = get(handles.cb_Play,'Value');
D3d.Viewer.Play(on);
D3d.Update();
end

% --- Executes on button press in cb_Rotate.
function cb_Rotate_Callback(hObject, eventdata, handles)
on = get(handles.cb_Rotate,'Value');
D3d.Viewer.Spin(on);
D3d.Update();
end

% --- Executes on button press in pb_ResetView.
function pb_ResetView_Callback(hObject, eventdata, handles)
D3d.Viewer.ResetView();
D3d.Update();
end

% --- Executes on button press in cb_SegmentationResults.
function cb_SegmentationResults_Callback(hObject, eventdata, handles)
on = get(handles.cb_SegmentationResults,'Value');
D3d.Viewer.ShowAllPolygons(on);
D3d.Update();
end

% --- Executes on button press in cb_ShowLabels.
function cb_ShowLabels_Callback(hObject, eventdata, handles)
on = get(handles.cb_ShowLabels,'Value');
D3d.Viewer.ShowLabels(on);
D3d.Update();
end

% --- Executes on button press in pb_saveSettings.
function pb_saveSettings_Callback(hObject, eventdata, handles)
D3d.SaveTransferFunction();
end

% --- Executes on button press in pb_loadSettings.
function pb_loadSettings_Callback(hObject, eventdata, handles)
D3d.LoadTransferFunction();
end

%% Create Functions

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
function s_curFrame_CreateFcn(hObject, eventdata, handles)
% hObject    handle to s_curFrame (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background.
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
end
