classdef (Abstract,Sealed) Viewer
methods (Static)
    commandInfo = Info()
    Help(command)
    Init(arrowFaces,arrowVerts,arrowNorms,sphereFaces,sphereVerts,shereNorms,pathStr)
    shapeElement = MakeBallMask(radius)
end
methods (Static, Access = private)
    varargout = Mex(command, varargin)
end
end
