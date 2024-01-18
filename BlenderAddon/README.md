# Blender Add-on for Photon-v2

[//TODO]: # (add a tutorial on how to install)

The folder named `PhotonBlend` is a Blender add-on for Photon. **We support only Blender 2.92 and higher**. Please follow standard Blender add-on installation procedure to install it.

[//TODO]: # (simple tutorials on how to use the addon)

## Features

* Export scenes to Photon's scene description format
* Manipulate Photon materials with shader nodes
* Physically meaningful input parameters
* Preview and perform final render directly in Blender

## Development

* It can be useful to have [fake bpy modules](https://github.com/nutti/fake-bpy-module) around so that `import bpy` and its equivalent no longer trigger errors in your IDE (and autocomplete is now possible).
* By enabling `Edit > Preferences > Interface > Developer Extras` and `Python Tooltips`, you gain more information for developer from Blender's UI.
* To avoid opening Blender console manually everytime on startup, pass `-con` to the Blender executable.

## Troubleshooting

* Exported scene is different or missing some data.
  - If you are exporting via `File -> Export -> Photon Scene (.p2)`, try using the `Render to .p2` render engine to export instead. Unfortunately Blender currently does not support getting a depsgraph with `depsgraph.mode == 'RENDER'` (except the one passed to `bpy.types.RenderEngine` by Blender). This will cause the exporter to export in `VIEWPORT` mode, and many settings such as subdivision modifiers will not apply their `RENDER` mode settings. The exporter still attempts to enhance this part by trying to automatically adjust object settings to match the one used for final rendering (or what the user specifies), but chances are that some parts are still missing/different. The `Render to .p2` render engine export scenes in `RENDER` mode by default. See [Get "Render" dependency graph](https://devtalk.blender.org/t/get-render-dependency-graph/12164) for a related discussion.

* The render is completely black.
  - Go to `Window -> Toggle System Console` and see if there is message like "Warning: reading render result: expected channel "XXX" not found". If so, then it is due to we are not fully supporting Blender's layer system yet. Please change the view layer's name to its default (ViewLayer) for now.

* Camera view does not match what I see from the viewport.
  - Select the camera, set the property `Camera -> Sensor Fit` to `Horizontal` and see if that works for you. When the rendering has lower aspect ratio (e.g., height > width), `Auto` sensor fit seems to interpolating between vertical and horizontal sensor size/FoV. I have yet figured out how the actual interpolation is done.

## Known Issues

### Blender 3.6.5

* `File -> Export -> Photon Scene (.p2)` may omit a newly created material's node tree, resulting in a fallback material being used. Using fake user for the material or saving and reopening the .blend file seem to work. A better alternative is to use the `Render to .p2` render engine to export. This seems to have something to do with the depsgraph `VIEWPORT` & `RENDER` mode issue, `VIEWPORT` depsgraph does not evaluate a newly created material's node tree.

* Render to .p2 output empty (black) frames to output folder (`Output Properties -> Output -> Output Path`, defaults to `/tmp/`) no matter the render engine setting (`bl_use_image_save = False`). You may want to manually remove those files from time to time.
