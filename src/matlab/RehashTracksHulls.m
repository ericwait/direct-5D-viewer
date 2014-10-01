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

function RehashTracksHulls(trackID)
global Tracks Hulls

hulls = Tracks(trackID).hulls(Tracks(trackID).hulls > 0);
if (~isempty(hulls))
    [uniqueTimes, sortedIndcies] = unique([Hulls(hulls).frame]);
    if (length(uniqueTimes)~=length([Hulls(hulls).frame]))
        hullsString = sprintf('%d, ',Tracks(trackID).hulls);
        error('More than one hull on a frame on track %d\nHulls: %s',trackID,hullsString);
    end
    
    %Update the times from the sorting
    Tracks(trackID).startFrame = uniqueTimes(1);
    Tracks(trackID).endFrame = uniqueTimes(end);
    
    %Clear out the old list to the size that the zero padded list should be
    Tracks(trackID).hulls = zeros(1,uniqueTimes(end)-uniqueTimes(1)+1);
    
    %Add the hulls back into the list in the approprite location
    hashedTimes = uniqueTimes - uniqueTimes(1) + 1; %get hashed times of the hull list
    Tracks(trackID).hulls(hashedTimes) = hulls(sortedIndcies); %place the hulls in sorted order into thier hashed locations
else
    %Clear out hulls and times
    Tracks(trackID).startFrame = [];
    Tracks(trackID).endFrame = [];
    Tracks(trackID).hulls = [];
end
end