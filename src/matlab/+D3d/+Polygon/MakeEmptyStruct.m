function [ poly ] = MakeEmptyStruct()
%[ poly ] = D3d.Polygon.MakeEmptyStruct()

poly = struct('index',{0}, 'frame',{0}, 'label',{''}, 'color',{[0,0,0]}, 'faces',{[]}, 'verts',{[]}, 'norms',{[]}, 'CenterOfMass',{[0,0,0]});
end

