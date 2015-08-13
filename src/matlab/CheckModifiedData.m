global Hulls Tracks Families

%for t=1:length(Tracks)
    for h = 1: length(Tracks(4).hulls)
        Hulls(Tracks(4).hulls(h)).color = Tracks(3).color;
        Hulls(Tracks(4).hulls(h)).track = 4;
    end
%end
