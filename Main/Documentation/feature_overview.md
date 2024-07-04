# Feature Overview {#main_feature_overview}

[TOC]

A quick overview of the features supported by Photon. Similar to [Project Structure](./Documentation/project_structure.md), this page can also serve as a handy reference for locating the necessary documentation or source code.

## Visualizer

Rendering is the process of visualizing a scene. In Photon, we utilize a [Visualizer](@ref ph::Visualizer) to generate a [Renderer](@ref ph::Renderer), which is responsible for creating a rendered image. This is where most of the core algorithms meet and work together, making it arguably the most complex part of the render engine. Photon offers a variety of renderers, each tailored to handle specific scenarios. There is no one-size-fits-all renderer for any types of scenes, as the effectiveness of a rendering algorithm depends heavily on the scene's characteristics, such as geometry dimensions and material properties. At the extreme end, one can always construct a scene that will fail a certain rendering algorithm. Therefore, it is crucial to understand and experiment with different renderers when faced with rendering challenges.

### Unidirectional Path Tracing {#bvpt}

![BVPT Teaser](Gallery/055_books_based_on_Libri_sulla_mensola_books_by_archemi.jpg "An example image rendered using this renderer.")

This is a relatively straightforward approach to solving the *rendering equation* as proposed by Kajiya @cite Kajiya:1986:Rendering, and is often a good starting point before diving into more complex solvers. Backward unidirectional path tracing, however, has its limitations. It does not handle scenes with small light sources or scenes where light transport paths are difficult to trace. This renderer can be constructed by creating a [PathTracingVisualizer](@ref ph::PathTracingVisualizer) paired with [BVPT](@ref ph::ERayEnergyEstimator) energy estimator. If the rendering focuses solely on direct lighting, you may opt for the [BVPTDL](@ref ph::ERayEnergyEstimator) estimator instead.

### Unidirectional Path Tracing with NEE {#bneept}

![BNEEPT Teaser](Gallery/bathroom_based_on_Salle_de_bain_by_nacimus.jpg "NEE can handle more complex scenes.")

With the addition of Next Event Estimation (NEE), unidirectional path tracing can become more adept at handling complex scenes. While this method largely builds on the same principle as the non-NEE variant, this renderer incorporates additional sampling techniques, as described by Veach @cite Veach:1995:Optimally, to improve the rendering of scenes with smaller light sources (as these scenes are often challenging for unidirectional path tracing). In a similar way, create a [PathTracingVisualizer](@ref ph::PathTracingVisualizer) with [BNEEPT](@ref ph::ERayEnergyEstimator) energy estimator to leverage the benefits of NEE.

### Vanilla Photon Mapping {#vpm}

![VPM Teaser](Example/vpm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 500M photons (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

It is worth to clarify that the project is named *Photon* not because the focus is on photon mapping techniques, but because photon is the elementary particle that transmit light, and the primary aim of this project is to solve light transport problems. That said, Photon does offer several photon mapping techniques, with the most fundamental one being vanilla photon mapping as introduced by Jensen @cite Jensen:1996:Global. Photon mapping techniques are distinguished by their ability to *merge* nearby light transport paths, and this form of path reuse allows them to excel on a wider range of lighting phenomena. You can get a vanilla photon mapping renderer by creating a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) and set it to [Vanilla](@ref ph::EPhotonMappingMode) mode.

### Progressive Photon Mapping {#ppm}

![PPM Teaser](Example/ppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

This renderer implements the PPM algorithm by Hachisuka et al. @cite Hachisuka:2008:Progressive This method improves upon vanilla photon mapping by tracing a fixed set of view vertices as the first pass, then progressively refine the estimation of the incoming energy on those vertices. The rendered result will be closer to ground truth with more view vertices (and the above image is rendered with maximum view path length = 10). 

One of the key advantages of progressive photon mapping methods is their ability to utilize an effectively infinite number of photons, which allows it to handle complex lighting scenarios more effectively than vanilla photon mapping. This capability leads to superior rendering results, especially for scenes dominated by specular-diffuse-specular paths.
Create a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Progressive](@ref ph::EPhotonMappingMode) mode to use this renderer.

### Stochastic Progressive Photon Mapping {#sppm}

![SPPM Teaser](Example/sppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

This renderer implements the SPPM algorithm by Hachisuka et al. @cite Hachisuka:2009:Stochastic, and is intended to serve as a reference implementation for more sophisticated techniques. The SPPM (and PPM) implementation in Photon tries to avoid tricks and shortcuts where possible, so the evaluated radiance is more likely to remain correct as the project evolves. As a result, this renderer is not optimized for speed and may not be suitable for scenarios where rendering time is a critical factor. Create a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Progressive](@ref ph::EPhotonMappingMode) mode to use this renderer.

### Probabilistic Progressive Photon Mapping {#pppm}

![PPPM Teaser](Example/pppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

This renderer is based on Knaus and Zwicker's paper @cite Knaus:2011:Progressive, which innovatively decouples any intermediate radiance estimate pass from its previous iterations. By isolating each pass's calculations, we can treat any photon mapping technique as a black box and parallelizes the calculation of each pass trivially. However, this approach comes with a trade-off: increased memory consumption. Because each iteration is treated independently, more memory is required to store the intermediate results. This makes our implementation of PPPM more memory-intensive compared to other photon mapping techniques. Create a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Progressive](@ref ph::EPhotonMappingMode) mode to use this renderer.

### Scheduler

Some [Visualizer](@ref ph::Visualizer)s can make use of a [WorkScheduler](@ref ph::WorkScheduler) to distribute rendering work across multiple processor cores. Different types of schedulers dispatch their work in different ways. When rendering an image, a scheduler will typically use the dimensions of the image and number of iterations to define the total volume of work.

|  Scheduler Type  |  Dispatch Pattern  |
| :--------------: | :----------------: |
| [Bulk](@ref ph::PlateScheduler): Layer by layer, each layer convers the whole image. | ![Bulk Scheduler](Illustration/Scheduler/bulk_concept.svg){html: width=50%} |
| [Stripe](@ref ph::StripeScheduler): Divide image into stripes. | ![Stripe Scheduler](Illustration/Scheduler/stripe_concept.svg){html: width=50%} |
| [Grid](@ref ph::GridScheduler): Step through evenly divided image row by row. | ![Grid Scheduler](Illustration/Scheduler/grid_concept.svg){html: width=50%} |
| [Tile](@ref ph::TileScheduler): Step through divided image row by row. | ![Tile Scheduler](Illustration/Scheduler/tile_concept.svg){html: width=50%} |
| [Spiral](@ref ph::SpiralScheduler): Step through the image cell by cell in a spiral shape. | ![Spiral Scheduler](Illustration/Scheduler/spiral_concept.svg){html: width=50%} |
| [SpiralGrid](@ref ph::SpiralGridScheduler): Similar to spiral scheduler, except that each cell is subdivided into smaller cells. | ![Spiral Grid Scheduler](Illustration/Scheduler/spiral_grid_concept.svg){html: width=50%} |

## Observer {#observer}

[Observer](@ref ph::Observer) plays an important role in capturing and recording a scene: it intercepts incoming energy and record it on its sensing device called [Film](@ref ph::Film)--so you can observe the virtual world that has been rendered. Observers can generate energy [Receiver](@ref ph::Receiver)s. A common example of an energy receiver is camera, which senses and records lighting as a photograph.

### Pinhole Camera {#pinhole}

![Pinhole Camera Teaser](Example/pinhole_camera_teaser.jpg "Image rendered by a pinhole camera. Notice the sharpness in foreground and background; there is no difference and it appears to be in focus everywhere.")

Photon currently supports two types of perspective cameras: [PinholeCamera](@ref ph::PinholeCamera) and [ThinLensCamera](@ref ph::ThinLensCamera). A pinhole camera is simply composed of a small aperture (the "pinhole", which serves as its lens system) and a film. Images captured by this camera is similar to how a normal human perceives the world but with several simplifications. Due to its simplicity, it is widely adopted in computer graphics industry. This kind of camera can be generated by creating a [SingleLensObserver](@ref ph::SingleLensObserver) with a zero lens radius.

### Thin Lens Camera {#thinlens}

![Thin Lens Camera Teaser](Example/thinlens_camera_teaser.jpg "Image rendered by a thin-lens camera. Notice the depth of field effect.")

For thin lens camera, as its name suggests, the lens system in this camera is assumed to be a single lens with negligible thickness. The biggest advantage of it is that depth of field effects are possible under this model (without introducing too many complexities). In the above render, depth of field is achieved by a 72 mm lens focusing on the first metallic monkey. This camera can be created similarly as [pinhole camera](@ref pinhole) by using a non-zero lens radius.

### Environment Camera

[//TODO]: <> (wip)

### Energy Measurement

[//TODO]: <> (wip)

## Image

[//TODO]: <> (wip)

### Texture

[//TODO]: <> (wip)

### Film

[//TODO]: <> (wip)

### Filtering

[//TODO]: <> (wip)

### Tone-mapping

[//TODO]: <> (wip)

### Reading and Writing

[//TODO]: <> (wip)

### Merging

[//TODO]: <> (wip)

## Material {#material}

We present material implementations with a professional test scene standardized by André Mazzone and Chris Rydalch @cite Mazzone:2023:Standard. This scene is specifically designed to facilitate the observation and comparison of material appearances. There is an inset object within the main model that has a 18% albedo (this value is the current VFX industry standard), which serves as a neutral color comparator. For scene dimensions and other parameters, the [Universal Scene Description Working Group](https://github.com/usd-wg) has a [nice entry](https://github.com/usd-wg/assets/tree/main/full_assets/StandardShaderBall) for it, which is more up-to-date than the [original paper](https://dl.acm.org/doi/10.1145/3610543.3626181).

Materials are represented by [material data containers](@ref ph::Material). These containers generate [SurfaceOptics](@ref ph::SurfaceOptics) to describe [SurfaceBehavior](@ref ph::SurfaceBehavior) and [VolumeOptics](@ref ph::VolumeOptics) to describe [VolumeBehavior](@ref ph::VolumeBehavior). Surface and volume behavior objects are typically defined with respect to a [Primitive](@ref ph::Primitive). See the [Geometry](@ref geometry) section for more details.

### Matte Opaque

A classic material that is present in almost every renderer, whether offline or real-time, is the Lambertian-based diffuse BRDF, first described over 200 years ago by Lambert @cite Lambert:1760:Photometria. A generalization of Lambert's model is also implemented, which is often referred to as the Oren-Nayar reflectance model @cite Oren:1994:Generalization. This model describes the surface as a collection of small Lambertian facets, each having a different orientation. These models are shown below:

| ![Lambertian Material](Example/lambertian_diffuse_0p5.jpg) | ![Oren Nayar Material](Example/oren_nayar_0p5_120deg.jpg) |
| :------------------: | :------------------: |
| **Left: Lambertian diffuse BRDF with 50% albedo. Right: Oren-Nayar diffuse BRDF with 50% albedo and 120° facet standard deviation.** ||

### Ideal Substance

[//TODO]: <> (wip)

### Ideal Substance

[//TODO]: <> (wip)

### Abraded Opaque

[//TODO]: <> (wip)

### Abraded Translucent

[//TODO]: <> (wip)

### Binary Mixed Surface

[//TODO]: <> (wip)

### Layered Material

[//TODO]: <> (wip)

### Surface Behavior

[//TODO]: <> (wip)

### Volume Behavior

[//TODO]: <> (wip)

## Geometry {#geometry}

To represent a ball in the scene, you can use a [sphere geometry](@ref ph::GSphere); for a piece of paper on a table, a [rectangle geometry](@ref ph::GRectangle) can model it pretty well. Geometries are the core components of a scene, providing the structural framework for all other elements like materials, lights and physical motions. In the following sections, we will explore some common types of geometries available in Photon, giving you a rough idea of how these foundational elements are constructed and used in rendering.

### Intersectable and Primitive

When a [Geometry](@ref ph::Geometry) is defined in the scene, it will be converted to an [Intersectable](@ref ph::Intersectable) before the actual rendering process begins. The renderer further treat some intersectable types as [Primitive](@ref ph::Primitive) from which a richer set of functionalities are expected. In a similar way, some intersectable types are being classified as [Intersector](@ref ph::Intersector) and they typically act as an aggregate of intersectables.

> [!note]
> While these distinctions are not crucial from a user's perspective, they are important for developers or researchers who wish to delve deeper into the source code. Understanding these classifications also makes engine modifications easier.

### Basic Shapes

These are common shapes to have in a renderer and are frequently found in other render engines as well. They are useful not only for modeling objects but also for defining the shape of light sources (or anything that requires a shape). Tessellation is also supported on some types of geometries. We start by introducing the rectangular shape, which can be created by [rectangle geometry](@ref ph::GRectangle).

![Rectangle Geometry](Example/rectangle.jpg "Rectangle lies on the XY-plane by default. With UV in [0, 1] across the whole surface.")

We also have freestanding triangular shape which can be created by [triangle geometry](@ref ph::GTriangle).

![Triangle Geometry](Example/triangle.jpg "Triangle has customizable UVs and normals.")

> [!note]
> It is not recommended to build complex shapes out of multiple freestanding triangle geometries unless you have good reasons to do so. It is highly memory inefficient.

There is also spherical shape created by [sphere geometry](@ref ph::GSphere). The sphere generated is a true sphere by default (not a triangle mesh counterpart).

![Sphere Geometry](Example/sphere.jpg "An example sphere shape.")

Using [cuboid geometry](@ref ph::GCuboid), you can build a cube or box shape. Cuboids are axis-aligned bounding boxes (AABB) in their local space and allow for variable extents, making them more flexible than standard cubes.

![Cuboid Geometry](Example/cuboid.jpg "A Cuboid (in fact a cube in this image).")

A bonus to have cuboids is that voxel games like [Minecraft](https://en.wikipedia.org/wiki/Minecraft) can be rendered easily. Below is a work-in-progress render of a Minecraft level parser that tries to translate in-game data into PSDL:

![Minecraft Example](Example/textured_mc.jpg "Test render of a Minecraft level parser.")

Almost all shapes are built from triangle meshes for a typical scene. Games, modeling programs and other applications typically use triangle mesh to represent arbitrary 3-D shapes. It is basically a collection of triangles grouped in a way that approximates a target shape. It can be created by [triangle mesh geometry](@ref ph::GTriangleMesh) or [polygon mesh](@ref ph::GPlyPolygonMesh). The polygon mesh variant is a more memory efficient representation of a triangle mesh and supports customizable vertex layout and arbitrary index bit precision. Binary storage format is also supported ([PLY format](https://en.wikipedia.org/wiki/PLY_(file_format))).

![Triangle Mesh Geometry](Example/triangle_mesh.jpg "Stanford bunny built from a triangle mesh.")

### Advanced Shapes

We also support some interesting shapes such as wave and fractals. These special geometries are sometimes useful for modeling a scene. One of them is the [Menger sponge](https://en.wikipedia.org/wiki/Menger_sponge), a famous fractal shape. It can be created by [Menger sponge geometry](@ref ph::GMengerSponge).

![Menger Sponge Geometry](Example/menger_sponge.jpg "A fractal geometry (Menger sponge).")

The wave shape is basically a cuboid with its top surface tessellated according to a superposition of 2-D sine and cosine functions. It can be created by [wave geometry](@ref ph::GWave).

![Wave Geometry](Gallery/045_water_dragon_color_light.jpg "A triangle mesh submerged inside a wave geometry."){html: width=80%}

These advanced shapes add versatility to scene modeling in Photon.

### Acceleration Structure

[//TODO]: <> (wip)

## Light

Lighting is a crucial component in rendering a virtual world. Photon provides various types of light sources and emphasizes the use of physically based units for input parameters. This approach is supported by three primary reasons:

* Photon is designed as a physically based renderer, using physically based inputs ensures a natural and accurate conveyance of intent.
* Users can expect consistent renderings between different versions of the software with identical inputs.
* It is easier to validate the results against real world measurements.

In Photon, lights are represented by a base actor type [ALight](@ref ph::ALight). These light actors will be converted to a series of cooked data, including [Primitive](@ref ph::Primitive) (the geometric shape of the light), [SurfaceOptics](@ref ph::SurfaceOptics) (the surface material of the light), [Emitter](@ref ph::Emitter) (the energy source of the light), etc. The following sections give a brief overview on the types of light sources implemented in Photon.

### Area Light

Photon currently supports [rectangular](@ref ph::ARectangleLight) and [spherical](@ref ph::ASphereLight) area lights. An [area light](@ref ph::AAreaLight) emits energy uniformly across its surface, producing a visually pleasing transition between umbra and penumbra. The amount of energy emitted is specified in [Watt](https://en.wikipedia.org/wiki/Watt).

![Rectangle Light](Example/rectangle_light_175W.jpg "A 175 W rectangle light.")

A spherical area light illuminating the same teapots:

![Sphere Light](Example/sphere_light_300W.jpg "A 300 W sphere light (1 m radius).")

### Point Light

In Photon, [point light](@ref ph::APointLight) is implemented as a special spherical area light. This may suprise some, as we treat point lights as a subtype of area lights. The rationale is that traditional point light introduces an additional singularity in the rendering equation, and we already have quite a lot of singularities in the system, e.g., pinhole camera and mirror reflection/transmission. Moreover, traditional point lights are not physically possible anyway. Each singularity often requires special routines or conditions to handle properly, and managing them all as the renderer's complexity grows can be cumbersome. The idea is to give point lights a finite surface area, typically a sphere with a 1- to 3-cm diameter resembling the size of common light bulbs. [Maxwell Renderer](http://www.nextlimit.com/maxwell/) has adopted a similar design decision, although the reason behind it may differ. Below is a 300 W point light, emitting roughly the same brightness as the 300 W sphere light shown earlier:

![Point Light](Example/point_light_300W.jpg "A 300 W point light (0.5 cm radius).")

### Model Light

[Model light](@ref ph::AModelLight) can be considered a superset of [area light](@ref ph::AAreaLight). A key difference between them is that while area light need to have a constant emission profile, model light lifted this limitation and allow using variable emission profiles (such as [images](@ref ph::Image)) on arbitrary [geometry](@ref ph::Geometry).

![Model Light](Example/model_light.jpg "A model light in action: the textured teapot is emitting energy from its surface.")

### IES Light Profiles

An IES light profile stores the distribution of emitted energy of a light fixture. The majority of the datasets are provided by lighting manufacturers and are particularly useful for interior design and architecture visualization. Most commercial renderers can parse and render IES-based lights, and even some game engines support them. Photon does not treat IES light profiles as energy distribution functions, rather, they are interpreted as energy attenuating filters (effectively normalizing energy values to [0, 1], see [IES attenuated light](@ref ph::AIesAttenuatedLight)). This approach allows users to adjust total energy emitted by a light source freely, without being constrained by the absolute energy values stroed in the IES light profile. Still, it is always possible to extract the maximum energy density from an IES data file then applying the attenuation to faithfully reproduce the light fixture in the renderer.

![IES Light](Example/ies_point_light_300W.jpg "An interesting IES light profile applied on a 300 W point light.")

### Sky Dome

A powerful method of lighting is image based techniques, also known as environment map or HDRI lighting. The main idea is to assume energy from far regions can be tabulated with direction vectors as entries. Lighting up a scene with HDRIs is usually the fastest and the most effective way to achieve natural-looking results. In photon, we call this type of light source a [dome light](@ref ph::ADome). For a nice collection of HDR environment maps, visit https://hdrihaven.com/. Shown below is the same demo scene lit with an [image dome](@ref ph::AImageDome) source:

![HDRI Light](Example/env_light.jpg "")

Another approach to lighting a scene is to physically model the energy coming from the sky. When it comes to sun-sky models, a classic example is the model by Preetham et al. @cite Preetham:1999:Practical This model approximates the absolute value of radiant energy coming from the sky using a set of formulae. Given the latitude, longitude, date and time of the target location, we can analytically generate the sun and sky using [Preetham dome](@ref ph::APreethamDome).

![Preetham Light](Example/preetham_light.jpg "New York City illuminated by the morning sunlight at Helsinki (60.2° N, 24.9° E) on Dec. 20.")

## Sample Source

[//TODO]: <> (wip)

### Sample Generator

[//TODO]: <> (wip)



