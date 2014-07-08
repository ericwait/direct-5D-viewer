function UpdateSegmentationResults(updateType)
global uiControlHandles Hulls 

switch updateType
    case 'off'
        set(uiControlHandles.cb_Wireframe,'Value',1,'Enable','off');
        set(uiControlHandles.cb_segLighting,'Enable','off');
        set(uiControlHandles.pb_selectedTrack,'Enable','off','BackgroundColor',[0.314,0.314,0.314]);
        set(uiControlHandles.tb_ParentTrack,'Enable','off');
        set(uiControlHandles.tb_ChildrenTracks,'Enable','off');
        set(uiControlHandles.tb_CurrentHull,'Enable','off');
        set(uiControlHandles.tb_Volume,'Enable','off');
        set(uiControlHandles.tb_CenterOfMass,'Enable','off');
        set(uiControlHandles.cb_ShowFamily,'Enable','off');
    case 'on'
        if (~isempty(Hulls))
            set(uiControlHandles.cb_SegmentationResults,'Value',1,'Enable','on');
            set(uiControlHandles.cb_Wireframe,'Value',1,'Enable','on');
            set(uiControlHandles.cb_segLighting,'Enable','on');
            set(uiControlHandles.pb_selectedTrack,'Enable','inactive');
            set(uiControlHandles.tb_ParentTrack,'Enable','on');
            set(uiControlHandles.tb_ChildrenTracks,'Enable','on');
            set(uiControlHandles.tb_CurrentHull,'Enable','on');
            set(uiControlHandles.tb_Volume,'Enable','on');
            set(uiControlHandles.tb_CenterOfMass,'Enable','on');
            set(uiControlHandles.cb_ShowFamily,'Enable','on');
            updateText();
        end
    case 'time'
        updateText();
end
end

function updateText()
global Hulls Tracks selectedHull trackHulls familyHulls uiControlHandles
if (~isempty(selectedHull) && selectedHull>0)
    time = str2double(get(uiControlHandles.tb_curFrame,'String'));
    searchHulls = [];
    if (Tracks(Hulls(selectedHull).track).startFrame <= time && Tracks(Hulls(selectedHull).track).endFrame >= time)
        searchHulls = trackHulls;
    elseif (get(uiControlHandles.cb_ShowFamily,'Value'))
        c = setxor(trackHulls,familyHulls);
        searchHulls = intersect(familyHulls,c);
    end
    for i=1:length(searchHulls)
        if (Hulls(searchHulls(i)).frame == time)
            setMetrics(searchHulls(i));
            break
        end
    end
end
end

function setMetrics(hull)
global Hulls Tracks uiControlHandles

curTrack = Hulls(hull).track;

set(uiControlHandles.pb_selectedTrack,'String',num2str(curTrack),'BackgroundColor',Hulls(hull).color);
set(uiControlHandles.tb_ParentTrack,'Enable','on','String',num2str(Tracks(curTrack).parentTrack));
childrenStr = '';
for j=1:length(Tracks(curTrack).childrenTracks)
    childrenStr = sprintf('%s%d,',childrenStr,Tracks(curTrack).childrenTracks(j));
end
set(uiControlHandles.tb_ChildrenTracks,'Enable','on','String',sprintf(childrenStr));
set(uiControlHandles.tb_CurrentHull,'Enable','on','String',num2str(hull));
set(uiControlHandles.tb_Volume,'Enable','on','String',num2str(length(Hulls(hull).pixels)));
comStr = sprintf('(%d,%d,%d)',round(Hulls(hull).centerOfMassOrg(1)),round(Hulls(hull).centerOfMassOrg(2)),round(Hulls(hull).centerOfMassOrg(3)));
set(uiControlHandles.tb_CenterOfMass,'Enable','on','String',comStr);
end
