function CheckMessage()
global uiHandle Hulls Tracks selectedHull uiFrameSlider uiFrameTb

msgs = lever_3d('poll');

for i=1:length(msgs)
    switch msgs(i).command
        case 'error'
            msg = sprintf('Error from C code: %s\n\tError Code:%f',msgs(i).message,msgs(i).val);
            errordlg(msg,'','modal');
            error(msg);
        case 'null'
            return
        case 'close'
            close(uiHandle);
        case 'cellSelected'
            selectedHull = msgs(i).val;
            if (val == -1)
                lever_3d('viewSegmentation',1);
            else
                lever_3d('displayHulls',Tracks(Hulls(msgs(i).val).track).hulls);
            end
        case 'timeChange'
            set(uiFrameTb,'String',num2str(msgs(i).val+1));
            set(uiFrameSlider,'Value', msgs(i).val+1);
        case 'centerSelectedCell'
            lever_3d('setViewOrigin',Hulls(selectedHull).centerOfMass);
    end
end
end

