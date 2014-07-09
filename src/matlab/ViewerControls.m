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

% Last Modified by GUIDE v2.5 08-Jul-2014 15:14:23

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
global imageData channelData uiControlHandles

uiControlHandles = handles;
channelData = [];

if isempty(imageData)
    return
end

if (exist([imageData.DatasetName '_Settings.mat'],'file'))
    load([imageData.DatasetName '_Settings.mat']);
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

procStr = {'Process image with...','Contrast Enhancement','Markov Random Fields Denoise','Segment','Distance Map'};
set(handles.m_imageProcessing,'String',procStr);

updateCurrentState(handles);
end

% --- Executes when user attempts to close figure1.
function figure1_CloseRequestFcn(hObject, eventdata, handles)
global tmr Hulls Tracks Families orgImage processedImage distanceImage imageData Costs Colors selectedHull uiTreeFig uiControlHandles
lever_3d('close');

stop(tmr);
delete(tmr);

close(uiTreeFig);

clear mex
Hulls = [];
Tracks = [];
Families = [];
orgImage = [];
processedImage = [];
distanceImage = [];
imageData = [];
Costs = [];
Colors = [];
selectedHull = [];
uiControlHandles = [];

delete(hObject);
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
if (1==get(handles.rb_Processed,'Value'))
    lever_3d('transferUpdate',channelData,'processed');
else
    lever_3d('transferUpdate',channelData,'original');
end
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
    save([imageData.DatasetName '_Settings.mat'],'channelData');
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
val = get(handles.s_peelSize,'Value');
lever_3d('peelUpdate',val);
end

% --- Executes on button press in cb_textureLighting.
function cb_textureLighting_Callback(hObject, eventdata, handles)
lever_3d('textureLightingUpdate',get(handles.cb_textureLighting,'Value'));
end

% --- Executes on button press in rb_orgImage.
function rb_orgImage_Callback(hObject, eventdata, handles)
set(handles.rb_Processed,'Value',0);
set(handles.rb_orgImage,'Value',1);

lever_3d('viewTexture','original'); %TODO can this be split out into channels?
end

% --- Executes on button press in rb_Processed.
function rb_Processed_Callback(hObject, eventdata, handles)
set(handles.rb_Processed,'Value',1);
set(handles.rb_orgImage,'Value',0);

lever_3d('viewTexture','processed'); %TODO can this be split out into channels?
end

% --- Executes on selection change in m_imageProcessing.
function m_imageProcessing_Callback(hObject, eventdata, handles)
global imageData orgImage processedImage distanceImage segImage channelData Hulls
chan = get(handles.m_channelPicker,'Value');
channelData(chan).alphaMod = get(handles.s_alpha,'Value');

processIdx = get(handles.m_imageProcessing,'Value');
processStr = get(handles.m_imageProcessing,'String');

if (isempty(processedImage))
    processedImage = orgImage;
end

processed = 0;
switch processStr{processIdx}
    case 'Contrast Enhancement'
        params = {'Gaussian Sigma in X','Gaussian Sigma in Y', 'Gaussian Sigma in Z', 'Median Filter in X', 'Median Filter in Y','Median Filter in Z'};
        diaTitle = 'Contrast Enhancement';
        def = {'100', '100', '100', '3', '3', '3'};
        response = inputdlg(params,diaTitle,1,def);
        if (~isempty(response))
            gX = str2double(response{1});
            gY = str2double(response{2});
            gZ = str2double(response{3});
            mX = str2double(response{4});
            mY = str2double(response{5});
            mZ = str2double(response{6});
            tic
            if (get(handles.rb_Processed,'Value')==1)
                for t=1:size(processedImage,5)
                    processedImage(:,:,:,chan,t) = CudaMex('ContrastEnhancement',processedImage(:,:,:,chan,t),[gX,gY,gZ],[mX,mY,mZ]);
                end
            else
                for t=1:size(orgImage,5)
                    processedImage(:,:,:,chan,t) = CudaMex('ContrastEnhancement',orgImage(:,:,:,chan,t),[gX,gY,gZ],[mX,mY,mZ]);
                end
            end
            processTime = toc;
            fprintf('Contrast Enhancement took: %f total sec, or %f avg per frame\n',processTime,processTime/size(processedImage,5));
            processed = 1;
        end
    case 'Markov Random Fields Denoise'
        params = {'Max iterations'};
        diaTitle = 'Denoise';
        def = {'100'};
        response = inputdlg(params,diaTitle,1,def);
        if (~isempty(response))
            iter = str2num(response{1});
            tic
            if (get(handles.rb_Processed,'Value')==1)
                for t=1:size(processedImage,5)
                    processedImage(:,:,:,chan,t) = uint16(CudaMex('MarkovRandomFieldDenoiser',single(processedImage(:,:,:,chan,t)),iter));
                end
            else
                for t=1:size(orgImage,5)
                    processedImage(:,:,:,chan,t) = uint16(CudaMex('MarkovRandomFieldDenoiser',single(orgImage(:,:,:,chan,t)),iter));
                end
            end
            processed = 1;
            processTime = toc;
            fprintf('Markov Random Fields Denoise: %f total sec, or %f avg per frame\n',processTime,processTime/size(processedImage,5));
        end
    case 'Segment'
        params = {'alpha','Opening Radius in X','Opening Radius in Y','Opening Radius in Z','Min Cell Diameter'};
        diaTitle = 'Segmentation';
        def = {'1.0', '2', '2', '1','6'};
        response = inputdlg(params,diaTitle,1,def);
        if (~isempty(response))
            alpha = str2double(response{1});
            oX = str2double(response{2});
            oY = str2double(response{3});
            oZ = str2double(response{4});
            dia = str2double(response{5});
            tic
            if (isempty(segImage))
                segImage = zeros(size(processedImage),'uint8');
            end
            for t=1:size(processedImage,5)
                segImage(:,:,:,chan,t) = CudaMex('Segment',processedImage(:,:,:,chan,t),alpha,[oX,oY,oZ]);
            end
            processTime = toc;
            fprintf('Image Processing Took: %f total sec, or %f avg per frame\n',processTime,processTime/size(processedImage,5));
            Segment(chan,dia);
            if (~isempty(distanceImage))
                set(handles.m_DistanceChoice,'Enable','on');
            end
        end
    case 'Distance Map'
        params = {'alpha','Opening Radius in X','Opening Radius in Y','Opening Radius in Z'};
        diaTitle = 'Distance Map';
        def = {'1.0', '2', '2', '1'};
        response = inputdlg(params,diaTitle,1,def);
        if (~isempty(response))
            alpha = str2double(response{1});
            oX = str2double(response{2});
            oY = str2double(response{3});
            oZ = str2double(response{4});
            if (isempty(distanceImage))
                distanceImage = zeros(size(processedImage));
            end
            tic
            for t=1:size(processedImage,5)
                temp = CudaMex('Segment',processedImage(:,:,:,chan,t),alpha,[oX,oY,oZ]);
                temp = temp>=max(temp(:));
                distanceImage(:,:,:,chan,t) = bwdist(temp,'euclidean'); %TODO make this in Cuda for anisotropic images
            end
            processTime = toc;
            fprintf('Distance Map: %f total sec, or %f avg per frame\n',processTime,processTime/size(processedImage,5));
            if (~isempty(Hulls))
                SetDistances(chan);
                set(handles.m_DistanceChoice,'Enable','on');
            end
        end
end

if (processed>0)    
    set(handles.rb_Processed,'Enable','on','Value',1);
    set(handles.rb_orgImage,'Value',0);
    
%     channelData(chan).alphaMod = 1.0;
%     channelData(chan).minVal = 0.0;
%     channelData(chan).midVal = 0.5;
%     channelData(chan).maxVal = 1.0;
%     channelData(chan).a = 0.0;
%     channelData(chan).b = 1.0;
%     channelData(chan).c = 0.0;
    
    viewImage = zeros(size(processedImage),'uint8');
    for c=1:size(processedImage,4)
        mx = max(processedImage(:,:,:,c,:));
        mx = double(max(mx(:)));
        viewImage(:,:,:,c,:) = uint8(double(processedImage(:,:,:,c,:)) ./mx .*255);
    end
    
    lever_3d('loadTexture',imageConvert(viewImage,'uint8'),[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,imageData.ZPixelPhysicalSize],'processed');
    updateCurrentState(handles);
end

if (~isempty(Hulls))
    set(handles.cb_SegmentationResults,'Value',1,'Enable','on');
    set(handles.cb_Wireframe,'Value',1,'Enable','on');
    set(handles.cb_segLighting,'Enable','on');
end

set(handles.m_imageProcessing,'Value',1);
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

% --- Executes on button press in cb_CaptureMovie.
function cb_CaptureMovie_Callback(hObject, eventdata, handles)
%TODO capture movie button
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
global Hulls Tracks Families imageData
folderName = uigetdir();
if (folderName == 0), return, end

save(fullfile(folderName,[imageData.DatasetName '_Segmenation.mat']),'Hulls','Tracks','Families','-v7.3');
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

if (exist(fullfile(folderName,'processed'),'dir'))
    processedImage = tiffReader('',[],[],[],fullfile(folderName,'processed'));
    lever_3d('loadTexture',imageConvert(processedImage,'uint8'),[imageData.XPixelPhysicalSize,imageData.YPixelPhysicalSize,...
        imageData.ZPixelPhysicalSize],'processed');
    set(handles.rb_Processed,'Enable','on','Value',1);
    set(handles.rb_orgImage,'Value',0);
    updateCurrentState(handles);
end
if (exist(fullfile(folderName,'segmentation'),'dir'))
    segImage = tiffReader('',[],[],[],fullfile(folderName,'segmentation'));
end
if (exist(fullfile(folderName,sprintf('%s_distanceMaps.mat',imageData.DatasetName)),'file'))
    load(fullfile(folderName,sprintf('%s_distanceMaps.mat',imageData.DatasetName)));
    if (~isempty(Hulls))
        SetDistances(chan);
        set(handles.m_DistanceChoice,'Enable','on');
    end
end
end

% --- Executes on button press in pb_OpenSegmentation.
function pb_OpenSegmentation_Callback(hObject, eventdata, handles)
global Hulls Tracks Families distanceImage
[fileName, pathName, ~] = uigetfile('.mat');
if (fileName == 0), return, end

load(fullfile(pathName,fileName));
if (~isempty(Hulls))
    set(handles.cb_SegmentationResults,'Value',1,'Enable','on');
    set(handles.cb_Wireframe,'Value',1,'Enable','on');
    set(handles.cb_segLighting,'Enable','on');
    
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
figure1_CloseRequestFcn();
end

% --- Executes on selection change in m_DistanceChoice.
function m_DistanceChoice_Callback(hObject, eventdata, handles)
global useDistance Hulls
chan = get(handles.m_channelPicker,'Value');
SetDistances(chan);

useDistance = get(handles.m_DistanceChoice,'Value')-1;
DrawTree();
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
