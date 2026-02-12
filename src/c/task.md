# Tasks

- [/] Analyze Interface Requirements <!-- id: 0 -->
    - [x] Review `MexCommand.h` and `MexCommand.cpp` to understand the command pattern. <!-- id: 1 -->
    - [x] Review `MexInit.cpp` to understand initialization. <!-- id: 2 -->
    - [x] Review `MexLoadTextureFrame.cpp` (already viewed, but keep in mind) for data transfer. <!-- id: 3 -->
- [ ] Design Python Architecture <!-- id: 4 -->
    - [x] Determine best binding tool (PyBind11 recommended). <!-- id: 5 -->
    - [x] Map Matlab `Mex*` classes to Python functions. <!-- id: 6 -->
    - [x] Plan data conversion (Numpy -> C++ pointers). <!-- id: 7 -->
- [ ] Implement Python Interface <!-- id: 8 -->
    - [ ] Create `src/c/Python` directory structure. <!-- id: 9 -->
    - [ ] Write `src/c/Python/PyBind.cpp` with core function bindings. <!-- id: 10 -->
    - [ ] Updates `src/c/CMakeLists.txt` to include Python target. <!-- id: 11 -->
    - [ ] Create Python package structure in `src/python/`. <!-- id: 12 -->
    - [ ] Verify build and basic import. <!-- id: 13 -->
- [ ] Implement Universal UI (Phase 2) <!-- id: 14 -->
    - [ ] Add `imgui` to `src/c/D3dLib/CMakeLists.txt`. <!-- id: 15 -->
    - [ ] Initialize ImGui in `Renderer.cpp`. <!-- id: 16 -->
    - [ ] Port `Controls.m` UI logic to C++ ImGui calls. <!-- id: 17 -->
