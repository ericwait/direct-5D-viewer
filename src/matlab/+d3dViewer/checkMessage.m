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

function checkMessage(src, evnt)
global uiControlFig Hulls selectedHull trackHulls familyHulls uiControlHandles

persistent shift ctrl alt selectedHullsList
msgs = d3dViewer.d3dViewer('poll');

for i=1:length(msgs)
    switch msgs(i).command
        case 'error'
            msg = sprintf('Error from C code: %s\n\tError Code:%f',msgs(i).message,msgs(i).val);
            errordlg(msg,'','modal');
            warning(msg);
        case 'null'
            return
        case 'close'
            d3dViewer.close();
%         case 'cellSelected'
%             if (msgs(i).val == -1)
%                 lever_3d('viewSegmentation',1);
%                 trackHulls = [];
%                 familyHulls = [];
%             else
%                 selectedHull = msgs(i).val;
%                 if (shift)
%                     selectedHullsList = [selectedHullsList selectedHull];
%                     lever_3d('displayHulls',selectedHullsList);
%                 else
%                     selectedHullsList = [];
%                     [trackHulls, familyHulls] = GetHullsToDisplay(selectedHull);
%                     if (get(uiControlHandles.cb_ShowFamily,'Value')==1)
%                         lever_3d('displayHulls',familyHulls);
%                     else
%                         lever_3d('displayHulls',trackHulls);
%                     end
%                     DrawTree();
%                 end
%             end
%         case 'timeChange'
%             UpdateTime(msgs(i).val,1);
%         case 'play'
%             set(uiControlHandles.cb_Play,'Value',msgs(i).val);
%         case 'rotate'
%             set(uiControlHandles.cb_Rotate,'Value',msgs(i).val);
%         case 'centerSelectedCell'
%             lever_3d('setViewOrigin',Hulls(selectedHull).centerOfMass);
%         case 'toggleLabels'
%             set(uiControlHandles.cb_ShowLabels,'Value',~get(uiControlHandles.cb_ShowLabels,'Value'));
%         case 'toggleHulls'
%             on = ~get(uiControlHandles.cb_SegmentationResults,'Value');
%             set(uiControlHandles.cb_SegmentationResults,'Value',on);
%             lever_3d('viewSegmentation',on);
%             if (on)
%                 UpdateSegmentationResults('on');
%                 DrawTree();
%             else
%                 UpdateSegmentationResults('off');
%             end
%         case 'keyDown'
%             if (strcmp(msgs(i).message,'shift'))
%                 shift = 1;
%             elseif (strcmp(msgs(i).message,'ctrl'))
%                 ctrl = 1;
%             elseif (strcmp(msgs(i).message,'alt'))
%                 alt = 1;
%             elseif (strcmp(msgs(i).message,'number'))
%                 num = msgs(i).val;
%                 if (~isempty(selectedHull) && selectedHull>0 && num>1)
%                     SplitHull(selectedHull,num);
%                 end
%             end
%         case 'keyUp'
%             if (strcmp(msgs(i).message,'shift'))
%                 shift = 0;
%             elseif (strcmp(msgs(i).message,'ctrl'))
%                 ctrl = 0;
%             elseif (strcmp(msgs(i).message,'alt'))
%                 alt = 0;
%             end
    end
end
end

