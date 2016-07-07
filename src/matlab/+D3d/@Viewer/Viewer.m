classdef (Abstract,Sealed) Viewer
methods (Static)
    commandInfo = Info()
    Help(command)
    AddPolygons(polygonsStruct)
    CaptureImage()
    CaptureSpinMovie()
    Close()
    DeleteAllPolygons()
    DisplayPolygons()
    Init(arrowFaces,arrowVerts,arrowNorms,sphereFaces,sphereVerts,shereNorms,pathStr)
    LoadTexture(Image,AnisotropicVoxelDimension,BufferType)
    PeelUpdate(PeelSize)
    Play(playOn)
    MessageArray = Poll()
    ReleaseControl()
    RemovePolygon()
    ResetView()
    Rotate()
    PolygonLighting()
    SetBackgroundColor()
    SetCapturePath()
    SetFrame()
    SetViewOrigin(viewOrigin)
    SetWindowSize(width,height)
    ShowLabels()
    ShowPolygons(polygonIdxList)
    ShowTexture(bufferType)
    TakeControl()
    TextureAttenuation()
    TextureLighting(lightOn)
    TransferFunction(TransferFunctionStruct,BufferType)
    ToggleWireframe(wireFrameOn)
end
methods (Static, Access = private)
    varargout = Mex(command, varargin)
end
end
