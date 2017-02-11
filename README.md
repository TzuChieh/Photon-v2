## What is Photon-v2?
Photon-v2 is a rendering engine made out of my personal interest in computer graphics. A rendering engine is what takes geometrical descriptions and material properties of a 3-D scene as input, and outputs a 2-D image of that scene from a specific point of view. In layman's terms, it is like taking a photograph, except the scene and the camera are both computer simulated. Different from its previous version, Photon, which was written entirely in Java, Photon-v2 utilizes the powerful C++14 for the rendering core and builds a GUI on top of it with Java via JNI. This project is currently in development and more features are expected to come. <br />

## Features
* **Integration (rendering method)**
 * backward path tracing
 * backward light tracing (next event estimation)
 * MIS'ed backward tracing (path + light)
 * light/particle tracing
* **Material**
 * Lambertian diffuse
 * microfacet-based opaque and translucent model
* **Geometry**
 * sphere shape
 * rectangular shape
 * triangle mesh
* **Light**
 * area light
* **Misc.**
 * Blender addon for scene creation

## Build
Photon-v2 is being developed with cross-platformity in mind, but Windows is the only platform that has been tested, for now. <br />

(building instructions will be added soon...) <br />

## Images Rendered by Photon-v2
| <a href="url"><img src="./gallery/028_sponza gold cups 12800spp.png" align="left" width="450" ></a> | <a href="url"><img src="./gallery/038_cbox 3 objs 11000spp.png" align="left" width="450" ></a> |
| --- | --- |
| <a href="url"><img src="./gallery/044_scenery glass dragon 6000spp.png" align="left" width="450" ></a> | <a href="url"><img src="./gallery/045_water dragon color light.png" align="left" width="450" ></a> |

## Scene Description Language
The scene descripting language (SDL) designed by T.C. Chang can be stored into a text file (.p2 filename extension) and loaded by Photon-v2 for rendering. To create a scene, you can either open up a text file and start writing right away (visit folder "./scenes/" for reference) or install the Blender addon (in "./BlenderAddon/"), which contains a .p2 file exporter and a material editor currently.

## About the Author
Development Blog: http://tcchangs.blogspot.tw/ <br />
YouTube Channel:  https://www.youtube.com/channel/UCKdlogjqjuFsuv06wajp-2g <br />
Facebook Page:    https://www.facebook.com/tokzin3d/ <br />
E-mail:           b01502051@ntu.edu.tw <br />

If you have any questions or ideas about my project, feel free to send me an e-mail (= <br />
