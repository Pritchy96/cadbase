# Generic Graphics Framework
MkII of a Generic Graphics Framework, designed with a view to eventually expand it into a FOSS CAD program.

#   Current Features
This is an incomplete list - for now, please refer to either the codebase or the git commit messages for a full feature list (or ask me). If and when this project gains steam, a more complete feature list will be maintained.

    - Master Geometry list rendered to multiple GL Renderables - one for each window.
    - Fully realised Arcball camera - Rotate, Pan, Zoom.
    - Movable, dockable, independent windows.
    - Import Geometry from a variety of standard 3D file formats using Assimp - .obj, .blend, .3ds etc.
    - Master Geometry <-> Renderable linking allows for granular control of what the user sees; a hide boolean in the geometry object hides the object in all viewports, conversely a hide bool in the renderable hides the object in that viewport alone.
    - Isometric/Perspective camera toggling.
    - Toggleable grid.

#   Requirements
     git
     cmake
     g++
     zenity (for Linux)

# Building from source
```console
$ git clone --recursive https://github.com/Pritchy96/cadbase.git
$ cd cad-base/
$ mkdir build
$ cd build
$ cmake ..
$ make/ninja/etc
```
