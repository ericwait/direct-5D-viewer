function MipButtonUp( hObject,callbackdata )
global MipAxesHandle MipPrevRectangleHandle MipDragRectangleHandle MipAnnotationLeftHandle MipAnnotationRightHandle MipDragLineHangle

ud = get(MipAxesHandle,'UserData');
ud.IsDown = false;

set(MipAxesHandle,'UserData',ud);
oldPnt_xy = ud.PointDown_xy;

mousePnt = get(MipAxesHandle,'CurrentPoint');
newPnt_xy = mousePnt(1,:);

dists_xy = newPnt_xy-oldPnt_xy;

if (any(abs(dists_xy)>1023))
    % making too big of an ROI to send to D3d viewer
    if (dists_xy(1)>1023)
        oldPnt_xy(1) = min(ud.ImData.Dimensions(1), oldPnt_xy(1) + (dists_xy(1) - 1023));
    elseif (dists_xy(1)<-1023)
        oldPnt_xy(1) = max(1, oldPnt_xy(1) + (dists_xy(1) + 1023));
    end
    
    if (dists_xy(2)>1023)
        oldPnt_xy(2) = min(ud.ImData.Dimensions(2), oldPnt_xy(2) + (dists_xy(2) - 1023));
    elseif (dists_xy(2)<-1023)
        oldPnt_xy(2) = max(1, oldPnt_xy(2) + (dists_xy(2) + 1023));
    end
    
    rad = 1023;
else
    rad = max(abs(dists_xy));
end

startPnt = oldPnt_xy - rad;

set(MipDragRectangleHandle,'Visible','on','Position',[startPnt(1:2),2*rad,2*rad]);

send = questdlg('Send this Region of Intrest to the 3-D viewer?','Select ROI?','Yes','No','Yes');

if (strcmp(send,'Yes'))
    pos = get(MipDragRectangleHandle,'Position');
    set(MipPrevRectangleHandle,'Position',pos,'Visible','on');
    
    fprintf('(%.0f, %.0f, %.0f, %.0f)\n',startPnt(1),startPnt(2),startPnt(1)+rad,startPnt(2)+rad);   
end

set(MipAnnotationLeftHandle,'Visible','off');
set(MipAnnotationRightHandle,'Visible','off');
set(MipDragRectangleHandle,'Visible','off');
set(MipDragLineHangle,'Visible','off');
end

