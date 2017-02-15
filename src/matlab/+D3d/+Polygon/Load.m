function Load(polygons)
    global D3dUICtrlHandles
    
    D3d.Viewer.AddPolygons(polygons);
    on = get(D3dUICtrlHandles.handles.cb_Wireframe,'Value');
    D3d.Viewer.ToggleWireframe(on);

    on = get(D3dUICtrlHandles.handles.cb_segLighting,'Value');
    D3d.Viewer.PolygonLighting(on);
    
    on = get(D3dUICtrlHandles.handles.cb_SegmentationResults,'Value');
    D3d.Viewer.ShowAllPolygons(on);
end

