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

function Check(src, evnt)
global D3dUICtrlHandles EXT_MESAGE_FUNC

msgs = D3d.Viewer('poll');

if (strcmp(msgs(1).command,'null'))
    return
end

for i=1:length(msgs)
    %% run these commands reguardless of what any other function does
    switch msgs(i).command
        case 'error'
            msg = sprintf('Error from C code: %s\n\tError Code:%f',msgs(i).message,msgs(i).val);
            errordlg(msg,'','modal');
            warning(msg);
        case 'close'
            D3d.Close();
        case 'timeChange'
            D3d.UI.Ctrl.UpdateTime(msgs(i).val,true);
        case 'play'
            if (~isempty(D3dUICtrlHandles))
                set(D3dUICtrlHandles.handles.cb_Play,'Value',msgs(i).val);
            end
        case 'rotate'
            if (~isempty(D3dUICtrlHandles))
                set(D3dUICtrlHandles.handles.cb_Rotate,'Value',msgs(i).val);
            end
    end
    
    if (~isempty(EXT_MESAGE_FUNC))
        %% pass the message on
        EXT_MESAGE_FUNC(msgs(i));
    else
        switch msgs(i).command
            case 'rightClick'
                if (msgs(i).val ~= -1)
                    fprintf('Right click value: %d\n',msgs(i).val);
                end
        end
    end
end
end
