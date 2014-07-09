function CheckMessage()
global uiFigureHandle Hulls selectedHull trackHulls familyHulls uiControlHandles

persistent shift ctrl alt selectedHullsList
msgs = lever_3d('poll');

for i=1:length(msgs)
    switch msgs(i).command
        case 'error'
            msg = sprintf('Error from C code: %s\n\tError Code:%f',msgs(i).message,msgs(i).val);
            errordlg(msg,'','modal');
            warning(msg);
        case 'null'
            return
        case 'close'
            close(uiFigureHandle);
        case 'cellSelected'
            selectedHull = msgs(i).val;
            if (msgs(i).val == -1)
                lever_3d('viewSegmentation',1);
                trackHulls = [];
                familyHulls = [];
            else
                if (shift)
                    selectedHullsList = [selectedHullsList selectedHull];
                    lever_3d('displayHulls',selectedHullsList);
                else
                    selectedHullsList = [];
                    [trackHulls, familyHulls] = GetHullsToDisplay(selectedHull);
                    if (get(uiControlHandles.cb_ShowFamily,'Value')==1)
                        lever_3d('displayHulls',familyHulls);
                    else
                        lever_3d('displayHulls',trackHulls);
                    end
                    DrawTree();
                end
            end
        case 'timeChange'
            UpdateTime(msgs(i).val,1);
        case 'play'
            set(uiControlHandles.cb_Play,'Value',msgs(i).val);
        case 'rotate'
            set(uiControlHandles.cb_Rotate,'Value',msgs(i).val);
        case 'centerSelectedCell'
            lever_3d('setViewOrigin',Hulls(selectedHull).centerOfMass);
        case 'keyDown'
            if (strcmp(msgs(i).message,'shift'))
                shift = 1;
            elseif (strcmp(msgs(i).message,'ctrl'))
                ctrl = 1;
            elseif (strcmp(msgs(i).message,'alt'))
                alt = 1;
            elseif (strcmp(msgs(i).message,'number'))
                num = msgs(i).val;
                if (~isempty(selectedHull) && selectedHull>0)
                    SplitHull(selectedHull,num);
                end
            end
        case 'keyUp'
            if (strcmp(msgs(i).message,'shift'))
                shift = 0;
            elseif (strcmp(msgs(i).message,'ctrl'))
                ctrl = 0;
            elseif (strcmp(msgs(i).message,'alt'))
                alt = 0;
            end
    end
end
end

