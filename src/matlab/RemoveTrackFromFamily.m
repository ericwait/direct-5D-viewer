% RemoveTrackFromFamily(trackID) only removes the track from the tracks
% list currently assosiated in the CellFamily entery and updated the other
% CellFamily fields.
% This is not intended to change the structure, just to keep the data
% correct

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%     Copyright 2011 Andrew Cohen, Eric Wait and Mark Winter
%
%     This file is part of LEVer - the tool for stem cell lineaging. See
%     https://pantherfile.uwm.edu/cohena/www/LEVer.html for details
% 
%     LEVer is free software: you can redistribute it and/or modify
%     it under the terms of the GNU General Public License as published by
%     the Free Software Foundation, either version 3 of the License, or
%     (at your option) any later version.
% 
%     LEVer is distributed in the hope that it will be useful,
%     but WITHOUT ANY WARRANTY; without even the implied warranty of
%     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%     GNU General Public License for more details.
% 
%     You should have received a copy of the GNU General Public License
%     along with LEVer in file "gnu gpl v3.txt".  If not, see 
%     <http://www.gnu.org/licenses/>.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function RemoveTrackFromFamily(trackID)

global Families Tracks
familyID = Tracks(trackID).family;
if(isempty(familyID)),return,end

index = Families(familyID).tracks==trackID;

%remove track
Families(familyID).tracks(index) = [];

%update times

[minimum index] = min([Tracks(Families(familyID).tracks).startFrame]);
Families(familyID).startFrame = minimum;
Families(familyID).endFrame = max([Tracks(Families(familyID).tracks).endFrame]);

%set new root
if(Families(familyID).rootTrack == trackID)
    Families(familyID).rootTrack = Families(familyID).tracks(index);
end
if(isempty(index))
    Families(familyID).tracks = [];
end
end
