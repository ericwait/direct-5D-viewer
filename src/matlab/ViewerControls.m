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

% Last Modified by GUIDE v2.5 26-Sep-2014 16:04:34

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
function varargout = ViewerControls(varargin)
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
global imageData channelData uiControlHandles

uiControlHandles = handles;

if isempty(imageData)
    return
end

if (exist([imageData.DatasetName '_Settings.mat'],'file'))
    load([imageData.DatasetName '_Settings.mat']);
end
    
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

if isempty(channelData)
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
            strng = {sprintf('Channel %d',i)};
        else
            strng = [strng; {sprintf('Channel %d',i)}];
        end
    end
end

set(handles.m_channelPicker,'string',strng);
if (imageData.NumberOfFrames > 1)
    set(handles.tb_numFrames,'string',num2str(imageData.NumberOfFrames));
    set(handles.s_curFrame,'Max',imageData.NumberOfFrames,'Min',1,'Enable','on','SliderStep',[1/imageData.NumberOfFrames, 0.1],'Value',1);
end
set(handles.tb_numChan,'string',num2str(imageData.NumberOfChannels));
set(handles.tb_title,'string',imageData.DatasetName);
set(handles.tb_curFrame,'string',num2str(1));
set(handles.tb_xDim,'string',num2str(imageData.XDimension));
set(handles.tb_yDim,'string',num2str(imageData.YDimension));
set(handles.tb_zDim,'string',num2str(imageData.ZDimension));
set(handles.tb_phyX,'string',num2str(imageData.XPixelPhysicalSize));
set(handles.tb_phyY,'string',num2str(imageData.YPixelPhysicalSize));
set(handles.tb_phyZ,'string',num2str(imageData.ZPixelPhysicalSize));

procStr = {'Process image with...','Contrast Enhancement','Markov Random Fields Denoise','Segment, Track, & Lineage','Distance Map'};
set(handles.m_imageProcessing,'String',procStr);

UI.UpdateCurrentState(handles);
end

function bSavingData = checkSavingDataGlobal()
global gSavingData

bSavingData = gSavingData;
end

% --- Executes when user attempts to close figure1.
function figure1_CloseRequestFcn(hObject, eventdata, handles)
global tmr Hulls Tracks Families orgMetadata processedMetadata segMetadata distMetadata imageData Costs Colors selectedHull uiTreeFig uiControlHandles segImage channelData familyHulls trackHulls useDistance distChanUsed gSavingData

if gSavingData == 0
    lever_3d('close');

    if ~isempty(tmr)
        stop(tmr);
        delete(tmr);
        tmr = [];
    end

    if ~isempty(uiTreeFig)
        close(uiTreeFig);
    end

    clear mex
    Hulls = [];
    Tracks = [];
    Families = [];
    imageData = [];
    Costs = [];
    Colors = [];
    selectedHull = [];
    uiControlHandles = [];
    segImage = [];
    channelData = [];
    familyHulls = [];
    trackHulls = [];
    useDistance = [];
    orgMetadata = [];
    processedMetadata = [];
    segMetadata = [];
    distMetadata = [];
    distChanUsed = [];

    delete(hObject);
else
   warndlg('Saving... Please wait.'); 
end
end

% --- Outputs from this function are returned to the command line.
function varargout = ViewerControls_OutputFcn(hObject, eventdata, handles) 
varargout{1} = handles.output;
end

%% Callback functions

% --- Executes on slider movement.
function s_curFrame_Callback(hObject, eventdata, handles)
frame = round(get(handles.s_curFrame,'Value'));
UpdateTime(frame-1);
end

% --- Executes on slider movement.
function s_ceil_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
mxVal = get(handles.s_ceil,'Value');
mnVal = get(handles.s_floor,'Value');

if (mxVal<=mnVal-0.01)
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

UI.PlotTransferFunctions(handles);
end

% --- Executes on slider movement.
function s_mids_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
channelData(chan).midVal = get(handles.s_mids,'Value');

UI.PlotTransferFunctions(handles);
end

% --- Executes on slider movement.
function s_floor_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
mxVal = get(handles.s_ceil,'Value');
mnVal = get(handles.s_floor,'Value');

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

UI.PlotTransferFunctions(handles);
end

% --- Executes on slider movement.
function s_alpha_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
channelData(chan).alphaMod = get(handles.s_alpha,'Value');
UI.PlotTransferFunctions(handles);
end

% --- Executes on selection change in m_channelPicker.
function m_channelPicker_Callback(hObject, eventdata, handles)
UI.UpdateCurrentState(handles);
end

% --- Executes on button press in pb_colorChooser.
function pb_colorChooser_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');

channelData(chan).color = uisetcolor(channelData(chan).color);
UI.UpdateCurrentState(handles);
UI.PlotTransferFunctions(handles);
end

% --- Executes on button press in cb_visible.
function cb_visible_Callback(hObject, eventdata, handles)
global channelData
chan = get(handles.m_channelPicker,'Value');
channelData(chan).visible = get(handles.cb_visible,'Value');

UI.PlotTransferFunctions(handles);
end

% --- Executes on slider movement.
function s_peelSize_Callback(hObject, eventdata, handles)
val = get(handles.s_peelSize,'Value');
lever_3d('peelUpdate',val);
end

% --- Executes on button press in cb_textureLighting.
function cb_textureLighting_Callback(hObject, eventdata, handles)
lever_3d('textureLightingUpdate',get(handles.cb_textureLighting,'Value'));
end

% --- Executes on button press in cb_atten.
function cb_atten_Callback(hObject, eventdata, handles)
lever_3d('textureAttenUpdate',get(handles.cb_atten,'Value'));
end

% --- Executes on button press in rb_orgImage.
function rb_orgImage_Callback(hObject, eventdata, handles)
set(handles.rb_Processed,'Value',0);
set(handles.rb_orgImage,'Value',1);

lever_3d('viewTexture','original'); %TODO can this be split out into channels?
UI.UpdateCurrentState(handles);
end

% --- Executes on button press in rb_Processed.
function rb_Processed_Callback(hObject, eventdata, handles)
set(handles.rb_Processed,'Value',1);
set(handles.rb_orgImage,'Value',0);

lever_3d('viewTexture','processed'); %TODO can this be split out into channels?
UI.UpdateCurrentState(handles);
end

% --- Executes on selection change in m_imageProcessing.
function m_imageProcessing_Callback(hObject, eventdata, handles)
global imageData orgMetadata processedMetadata segMetadata distMetadata channelData Hulls Tracks Families tmr
chan = get(handles.m_channelPicker,'Value');
channelData(chan).alphaMod = get(handles.s_alpha,'Value');

processIdx = get(handles.m_imageProcessing,'Value');
processStr = get(handles.m_imageProcessing,'String');

stop(tmr);

processed = false;

switch processStr{processIdx}
    case 'Contrast Enhancement'
        processed = ImProc.ContrastEnhancement(handles,chan);
    case 'Markov Random Fields Denoise'
        processed = ImProc.MRFDenoise(handles, chan);
    case 'Segment, Track, & Lineage'
        Seg.SegmentBlob(handles, chan);
    case 'Distance Map'
        ImProc.DistMap(handles, chan);
end

if (processed>0)
    set(handles.rb_Processed,'Enable','on','Value',1);
    set(handles.rb_orgImage,'Value',0);
    UI.UpdateCurrentState(handles);
end

if (~isempty(Hulls))
    set(handles.cb_SegmentationResults,'Value',1,'Enable','on');
    set(handles.cb_Wireframe,'Value',1,'Enable','on');
    set(handles.cb_segLighting,'Enable','on');
    set(handles.cb_ShowLabels,'Enable','on');
end

set(handles.m_imageProcessing,'Value',1);
start(tmr);
end

% --- Executes on button press in cb_Wireframe.
function cb_Wireframe_Callback(hObject, eventdata, handles)
on = get(handles.cb_Wireframe,'Value');
lever_3d('wireframeSegmentation',on);
end

% --- Executes on button press in cb_segLighting.
function cb_segLighting_Callback(hObject, eventdata, handles)
%TODO put seg lighting in the shader to be turned on and off
end

% --- Executes on button press in cb_Play.
function cb_Play_Callback(hObject, eventdata, handles)
on = get(handles.cb_Play,'Value');
lever_3d('play',on);
end

% --- Executes on button press in cb_Rotate.
function cb_Rotate_Callback(hObject, eventdata, handles)
on = get(handles.cb_Rotate,'Value');
lever_3d('rotate',on);
end

% --- Executes on button press in pb_CaptureMovie.
function pb_CaptureMovie_Callback(hObject, eventdata, handles)
lever_3d('captureSpinMovie');
end

% --- Executes on button press in pb_ResetView.
function pb_ResetView_Callback(hObject, eventdata, handles)
lever_3d('resetView');
end

% --- Executes on button press in cb_ShowFamily.
function cb_ShowFamily_Callback(hObject, eventdata, handles)
global trackHulls familyHulls
if (get(handles.cb_ShowFamily,'Value'))
    if (isempty(familyHulls))
        lever_3d('viewSegmentation',1);
    else
        lever_3d('displayHulls',familyHulls);
    end
else
    if (isempty(trackHulls))
        lever_3d('viewSegmentation',1);
    else
        lever_3d('displayHulls',trackHulls);
    end
end
end

% --- Executes on button press in cb_SegmentationResults.
function cb_SegmentationResults_Callback(hObject, eventdata, handles)
on = get(handles.cb_SegmentationResults,'Value');
lever_3d('viewSegmentation',on);
if (on)
    UpdateSegmentationResults('on');
    DrawTree();
else
    UpdateSegmentationResults('off');
end
end

% --- Executes on button press in pb_SaveSegmentation.
function pb_SaveSegmentation_Callback(hObject, eventdata, handles)
global Hulls Tracks Families segMetadata imageData gSavingData

gSavingData = 1;
save(fullfile(segMetadata.PathName,[imageData.DatasetName '_Segmenation.mat']),'Hulls','Tracks','Families','-v7.3');
save(fullfile(segMetadata.PathName,'segMetadata.mat'),'segMetadata');

msg = 'There are empty structures:';
if isempty(Hulls)
    msg = [msg;{'Hulls'}];
end
if isempty(Tracks)
    msg = [msg;{'Tracks'}];
end
if isempty(Families)
    msg = [msg;{'Families'}];
end

if (size(msg,1)>1)
    warndlg(msg);
end

gSavingData = 0;
end

% --- Executes on button press in pb_SaveImages.
function pb_SaveImages_Callback(hObject, eventdata, handles)
global imageData processedImage segImage distanceImage 
folderName = uigetdir();
if (folderName == 0), return, end
    
msg = 'There are empty images:';

if (~isempty(processedImage))
    mkdir(folderName,'processed');
    tiffWriter(processedImage,sprintf('%s\\%s',fullfile(folderName,'processed'),imageData.DatasetName),imageData);
else
    msg = [msg;{'Processed Images'}];
end
if (~isempty(segImage))
    mkdir(folderName,'segmentation');
    tiffWriter(segImage,sprintf('%s\\%s',fullfile(folderName,'segmentation'),imageData.DatasetName),imageData);
else
    msg = [msg;{'Segmentation Images'}];
end
if (~isempty(distanceImage))
    save(fullfile(folderName,sprintf('%s_distanceMaps.mat',imageData.DatasetName)),'distanceImage','-v7.3');
else
    msg = [msg;{'Distance Maps'}];
end
if (size(msg,1)>1)
    warndlg(msg);
end
end

% --- Executes on button press in pb_OpenImages.
function pb_OpenImages_Callback(hObject, eventdata, handles)
global processedImage segImage distanceImage imageData Hulls
folderName = uigetdir();
if (folderName == 0), return, end

if (exist(fullfile(folderName,'segmentation'),'dir'))
    segImage = tiffReader('',[],[],[],fullfile(folderName,'segmentation'));
end
if (exist(fullfile(folderName,sprintf('%s_distanceMaps.mat',imageData.DatasetName)),'file'))
    load(fullfile(folderName,sprintf('%s_distanceMaps.mat',imageData.DatasetName)));
    if (~isempty(Hulls))
        set(handles.m_DistanceChoice,'Enable','on');
    end
end
if (exist(fullfile(folderName,'processed'),'dir'))
    processedImage = tiffReader('',[],[],[],fullfile(folderName,'processed'));
    lever_3d('loadTexture',imageConvert(processedImage,'uint8'),[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,...
        imageData.ZPixelPhysicalSize],'processed');
    set(handles.rb_Processed,'Enable','on','Value',1);
    set(handles.rb_orgImage,'Value',0);
    UI.UpdateCurrentState(handles);
end
end

% --- Executes on button press in pb_OpenSegmentation.
function pb_OpenSegmentation_Callback(hObject, eventdata, handles)
global Hulls Tracks Families distanceImage hullChan
[fileName, pathName, ~] = uigetfile('.mat');
if (fileName == 0), return, end

load(fullfile(pathName,fileName));
if (~isempty(Hulls))
    set(handles.cb_SegmentationResults,'Value',1,'Enable','on');
    set(handles.cb_Wireframe,'Value',1,'Enable','on');
    set(handles.cb_segLighting,'Enable','on');
    set(handles.cb_ShowLabels,'Enable','on');
    
    lever_3d('loadHulls',Hulls);
    if (~isempty(distanceImage))
        set(handles.m_DistanceChoice,'Enable','on');
    end
    
    DrawTree();
end
end

% --- Executes on button press in pb_New.
function pb_New_Callback(hObject, eventdata, handles)
lever3d();
end

% --- Executes on button press in pb_Close.
function pb_Close_Callback(hObject, eventdata, handles)
figure1_CloseRequestFcn(hObject, eventdata, handles);
end

% --- Executes on selection change in m_DistanceChoice.
function m_DistanceChoice_Callback(hObject, eventdata, handles)
global useDistance distMetadata

chan = get(handles.m_channelPicker,'Value');
if (isempty(distMetadata))
    warndlg('There is no distance data for this channel.  Please process first');
    set(handles.m_DistanceChoice,'Value',1);
    return
elseif (distMetadata.ChanProcessed(chan))
    distIm = tiffReader(fullfile(distMetadata.PathName,distMetadata.FileName),[],chan);
    SetDistances(distIm);
    clear distIm
else
    warndlg('There is no distance data for this channel.  Please process first');
    set(handles.m_DistanceChoice,'Value',1);
    return
end

useDistance = get(handles.m_DistanceChoice,'Value')-1;
DrawTree();
end

% --- Executes on button press in cb_ShowLabels.
function cb_ShowLabels_Callback(hObject, eventdata, handles)
on = get(handles.cb_ShowLabels,'Value');
lever_3d('showLabels',on);
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
if isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor',[.9 .9 .9]);
end
end

% --- Executes during object creation, after setting all properties.
function m_imageProcessing_CreateFcn(hObject, eventdata, handles)
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end

% --- Executes during object creation, after setting all properties.
function m_DistanceChoice_CreateFcn(hObject, eventdata, handles)
% hObject    handle to m_DistanceChoice (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
end
