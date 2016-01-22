function MipButtonMotion( hObject,callbackdata )
global MipAxesHandle MipDragRectangleHandle MipDragLineHandle MipTextHandle

ud = get(MipAxesHandle,'UserData');
if (ud.IsDown)
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
    set(MipDragLineHandle,'Visible','on','XData',[oldPnt_xy(1),newPnt_xy(1)],'YData',[oldPnt_xy(2),newPnt_xy(2)]);
end
end

