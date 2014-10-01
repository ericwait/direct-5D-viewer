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

function [trackHulls, familyHulls] = GetHullsToDisplay(curHull)
global Hulls Tracks Families 

if (curHull<1 || curHull>length(Hulls))
    trackHulls = [];
    familyHulls = [];
    return
end

curTrack = Hulls(curHull).track;
curFamily = Tracks(curTrack).family;
trackList = Families(curFamily).tracks;

trackHulls = Tracks(curTrack).hulls;
familyHulls = trackHulls;

for i=1:length(trackList)
    if (trackList(i)==curTrack), continue, end
    
    familyHulls = [familyHulls Tracks(trackList(i)).hulls];
end
end

