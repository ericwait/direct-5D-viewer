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
persistent shift ctrl alt selectedHullsList
global D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    
    msgs = D3d.Viewer('poll');
    
    for i=1:length(msgs)
        switch msgs(i).command
            case 'error'
                msg = sprintf('Error from C code: %s\n\tError Code:%f',msgs(i).message,msgs(i).val);
                errordlg(msg,'','modal');
                warning(msg);
            case 'null'
                return
            case 'close'
                D3d.Close();
            case 'timeChange'
                UpdateTime(msgs(i).val,true);
            case 'play'
                set(D3dUICtrlHandles.handles.cb_Play,'Value',msgs(i).val);
            case 'rotate'
                set(D3dUICtrlHandles.handles.cb_Rotate,'Value',msgs(i).val);
            case 'keyDown'
                if (strcmp(msgs(i).message,'shift'))
                    shift = 1;
                elseif (strcmp(msgs(i).message,'ctrl'))
                    ctrl = 1;
                elseif (strcmp(msgs(i).message,'alt'))
                    alt = 1;
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
else
    warning('Timer needs to be closed because no control handle exist!');
end
end

