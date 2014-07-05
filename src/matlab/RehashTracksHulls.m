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