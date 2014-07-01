function CheckMessage()
global uiHandle Hulls Tracks selectedHull

[command, val] = lever_3d('poll');

switch command
    case 'null'
        return
    case 'close'
        close(uiHandle);
    case 'cellSelected'
        selectedHull = val;
        if (val == -1)
            lever_3d('viewSegmentation',1);
        else
            lever_3d('displayHulls',Tracks(Hulls(val).track).hulls);
        end
end
end

