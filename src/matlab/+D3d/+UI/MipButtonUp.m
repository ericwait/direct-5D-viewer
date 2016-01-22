function MipButtonUp( hObject,callbackdata )
global MipAxesHandle MipPrevRectangleHandle MipDragRectangleHandle MipDragLineHandle MipTextHandle MipFullIm MipROIim MipROIimData

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
    rad = floor(max(abs(dists_xy)));
end

startPnt_xy = ceil(oldPnt_xy - rad);

set(MipDragRectangleHandle,'Visible','on','Position',[startPnt_xy(1:2),2*rad,2*rad]);

send = questdlg('Send this Region of Intrest to the 3-D viewer?','Select ROI?','Yes','No','Yes');

if (strcmp(send,'Yes'))
    pos = get(MipDragRectangleHandle,'Position');
    set(MipPrevRectangleHandle,'Position',pos,'Visible','on');
    
    %fprintf('(%.0f, %.0f, %.0f, %.0f)\n',startPnt(1),startPnt(2),startPnt(1)+rad,startPnt(2)+rad);   
    if (isempty(MipFullIm))
        [MipROIim,MipROIimData] = MicroscopeData.ReaderParZ(ud.ImData.imageDir,[],[],[],[],[],true,[],startPnt_xy(1:2),[rad,rad]);
    else
        MipROIim = MipFullIm(startPnt_xy(2):startPnt_xy(2)+rad,startPnt_xy(1):startPnt_xy(1)+rad,:,:,:);
        MipROIimData = ud.ImData;
        MipROIimData.Dimensions = [size(MipROIim,2),size(MipROIim,1),size(MipROIim,3)];
    end
    
    [~,~,maxVal] = Utils.GetClassBits(MipROIim);
    
    goodZ = squeeze(max(max(max(MipROIim,[],1),[],2),[],4));
    zStart = find(goodZ>maxVal*0.04,1,'first');
    if (isempty(zStart))
        zStart = 1;
    end
    zEnd = find(goodZ>maxVal*0.04,1,'last');
    if (isempty(zEnd))
        zEnd = size(MipROIim,3);
    end
    MipROIim = MipROIim(:,:,zStart:zEnd,:,:);
    MipROIimData.Dimensions(3) = size(MipROIim,3);
    D3d.Close();
    D3d.Open(MipROIim,MipROIimData);
end

set(MipDragRectangleHandle,'Visible','off');
set(MipDragLineHandle,'Visible','off');
set(MipTextHandle,'Visible','off');
end

