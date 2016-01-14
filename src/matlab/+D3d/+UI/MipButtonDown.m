function MipButtonDown( hObject, callbackdata )
global MipAxesHandle

ud = get(MipAxesHandle,'UserData');

if (~ud.IsDown)
    % check to see if we missed the up event
    mousePnt = get(MipAxesHandle,'CurrentPoint');
    ud.PointDown_xy = mousePnt(1,:);
    ud.IsDown = true;
    
    set(MipAxesHandle,'UserData',ud);
end
end
