classdef (Abstract,Sealed) Viewer
methods (Static)
    commandInfo = Info()
    Help(command)
    AddPolygons(polygonsStruct)
    CaptureSpinMovie()
    Close()
    DeleteAllPolygons()
    DisplayPolygons(polygonIndices)
    Init(arrowFaces,arrowVerts,arrowNorms,sphereFaces,sphereVerts,shereNorms,pathStr)
    LoadTexture(Image,AnisotropicVoxelDimension,BufferType)
    PeelUpdate(PeelSize)
    Play(playOn)
    MessageArray = Poll()
    ReleaseControl()
    RemovePolygon()
    ResetView()
    Rotate(on)
    PolygonLighting(lightOn)
    SetBackgroundColor(color)
    SetCapturePath(filePath,filePrefix)
    SetFrame(frame)
    SetRotation(deltaXangle,deltaYangle,deltaZangle)
    SetViewOrigin(viewOrigin)
    SetWindowSize(width,height)
    ShowLabels(on)
    ShowPolygons(on)
    ShowTexture(bufferType)
    TakeControl()
    TextureAttenuation(on)
    TextureLighting(lightOn)
    TransferFunction(TransferFunctionStruct,BufferType)
    ToggleWireframe(wireFrameOn)
end
methods (Static, Access = private)
    varargout = Mex(command, varargin)
end
end
