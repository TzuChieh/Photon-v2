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

## Troubleshooting

* The render is completely black.
  - Go to `Window -> Toggle System Console` and see if there is message like "Warning: reading render result: expected channel "XXX" not found". If so, then it is due to we are not fully supporting Blender's layer system yet. Please change the view layer's name to its default (ViewLayer) for now.

* Camera view does not match what I see from the viewport.
  - Select the camera, set the property `Camera -> Sensor Fit` to `Horizontal` and see if that works for you. When the rendering has lower aspect ratio (e.g., height > width), `Auto` sensor fit seems to interpolating between vertical and horizontal sensor size/FoV. I have yet figured out how the actual interpolation is done.
