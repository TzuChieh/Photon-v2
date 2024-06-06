# Feature Overview

[TOC]

A quick overview of the features supported by Photon. Similar to [Project Structure](./Documentation/project_structure.md), this page can also serve as a handy reference for you to find the documentation or source code that you need.

## Visualizer

Rendering is the process of visualizing a scene. In Photon, we use a [Visualizer](@ref ph::Visualizer) to generate a [Renderer](@ref ph::Renderer) to actually render an image. This is where most of the core algorithms meet and work together--and is arguably the most complex part of a renderer. While there are many types of renderers to choose from, there is hardly an all-purpose renderer as each one of them is designed to deal with different situations. The efficiency of a rendering algorithm is also partly depending on the characteristics of a scene, e.g., sizes of geometries, material properties, etc. At the extreme end, one can always construct a scene that will fail a certain rendering algorithm. Be sure to understand and try different renderers if things do not work out.

### Unidirectional Path Tracing {#bvpt}

![BVPT Teaser](Gallery/055_books_based_on_Libri_sulla_mensola_books_by_archemi.jpg "An example image rendered using this renderer.")

Though this is a relatively simple renderer to solve the *rendering equation* as proposed by Kajiya @cite Kajiya:1986:Rendering, this renderer is a good starting point before diving into other solvers. Backward unidirectional path tracing does not handle small light sources well, and scenes with hard-to-find light transport paths are likely to fail, too. This renderer can be constructed by creating a [PathTracingVisualizer](@ref ph::PathTracingVisualizer) with [BVPT](@ref ph::ERayEnergyEstimator::BVPT) energy estimator (use [BVPTDL](@ref ph::ERayEnergyEstimator::BVPTDL) if only direct lighting is required).

### Unidirectional Path Tracing with NEE {#bneept}

![BNEEPT Teaser](Gallery/bathroom_based_on_Salle_de_bain_by_nacimus.jpg "NEE can handle more complex scenes.")

With the addition of Next Event Estimation (NEE), unidirectional path tracing can handle common scenes quite effectively. While largely based on the same principle as the non-NEE variant, this renderer combines additional sampling techniques @cite Veach:1995:Optimally to better handle smaller light sources. Similarly, create a [PathTracingVisualizer](@ref ph::PathTracingVisualizer) with [BNEEPT](@ref ph::ERayEnergyEstimator::BNEEPT) energy estimator to use this renderer.

### Vanilla Photon Mapping {#vpm}

![VPM Teaser](Example/vpm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 500M photons (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

It is worth to clarify that the project is named *Photon* not because the focus is on photon mapping techniques, but because photon is the elementary particle that transmit light, and the core of this project is to solve light transport problems. Nevertheless, several photon mapping techniques are available here, and the most fundamental one is vanilla photon mapping @cite Jensen:1996:Global. All photon mapping techniques are capable of *merging* nearby light transport paths in some way, and this form of path reuse allows them to excel on a wider variety of lighting phenomena. You can get a vanilla photon mapping renderer by creating a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Vanilla](@ref ph::EPhotonMappingMode::Vanilla) mode.

### Progressive Photon Mapping {#ppm}

![PPM Teaser](Example/ppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

This renderer implements the PPM algorithm by Hachisuka et al. @cite Hachisuka:2008:Progressive The first pass traces a fixed set of view vertices then the following iterative process will estimate the incoming energy on those vertices. The rendered result will be closer to ground truth with more view vertices (and the above image is rendered with maximum view path length = 10). The algorithm can leverage effectively infinite amount of photons to render a scene, achieving superior results then vanilla photon mapping. Create a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Progressive](@ref ph::EPhotonMappingMode::Progressive) mode to use this renderer.

### Stochastic Progressive Photon Mapping {#sppm}

![SPPM Teaser](Example/sppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

This renderer implements the SPPM algorithm by Hachisuka et al. @cite Hachisuka:2009:Stochastic, and is intended to serve as a reference implementation for other more sophisticated techniques. Tricks and shortcuts are avoided if possible, so the evaluated radiance is more likely to remain correct as the project evolves. Performance is not a major concern for this renderer, so do not use it in situations where time is valuable. Create a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Progressive](@ref ph::EPhotonMappingMode::StochasticProgressive) mode to use this renderer.

### Probabilistic Progressive Photon Mapping {#pppm}

![PPPM Teaser](Example/pppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

This renderer is based on Knaus and Zwicker's paper @cite Knaus:2011:Progressive, which decouples any intermediate radiance estimate from previous iterations. We can treat any photon mapping technique as a black box and parallelizes the calculation of each iteration trivially. However, this renderer consumes more memory in comparison with others. Create a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Progressive](@ref ph::EPhotonMappingMode::ProbabilisticProgressive) mode to use this renderer.

### Scheduler

Some [Visualizer](@ref ph::Visualizer)s can make use of a [WorkScheduler](@ref ph::WorkScheduler) to distribute rendering work over multiple processor cores. Different types of schedulers dispatch their work in different ways. When rendering an image, a scheduler will typically use the dimensions of the image and number of iterations to define the total volume of work.

|  Scheduler Type  |  Dispatch Pattern  |
| :--------------: | :----------------: |
| [Bulk](@ref ph::PlateScheduler): Layer by layer, each layer convers the whole image. | ![Bulk Scheduler](Illustration/Scheduler/bulk_concept.svg){html: width=50%} |
| [Stripe](@ref ph::StripeScheduler): Divide image into stripes. | ![Stripe Scheduler](Illustration/Scheduler/stripe_concept.svg){html: width=50%} |
| [Grid](@ref ph::GridScheduler): Step through evenly divided image row by row. | ![Grid Scheduler](Illustration/Scheduler/grid_concept.svg){html: width=50%} |
| [Tile](@ref ph::TileScheduler): Step through divided image row by row. | ![Tile Scheduler](Illustration/Scheduler/tile_concept.svg){html: width=50%} |
| [Spiral](@ref ph::SpiralScheduler): Step through the image cell by cell in a spiral shape. | ![Spiral Scheduler](Illustration/Scheduler/spiral_concept.svg){html: width=50%} |
| [SpiralGrid](@ref ph::SpiralGridScheduler): Similar to spiral scheduler, except that each cell is subdivided into smaller cells. | ![Spiral Grid Scheduler](Illustration/Scheduler/spiral_grid_concept.svg){html: width=50%} |

## Observer

[Observer](@ref ph::Observer) plays an important role in a scene: it captures incoming energy and record it on its sensing device called [Film](@ref ph::Film)--so you can observe the virtual world which just got rendered. A common example of an observer is camera, which senses and records lighting as a photograph.

### Pinhole Camera

![Pinhole Camera Teaser](Example/pinhole_camera_teaser.jpg "Image rendered by a pinhole camera. Notice the sharpness in foreground and background; there is no difference and it appears to be in focus everywhere.")

Photon currently supports two types of perspective camera: [PinholeCamera](@ref ph::PinholeCamera) and [ThinLensCamera](@ref ph::ThinLensCamera). A pinhole camera is simply composed of a hole (which serves as its lens system) and a film. Images captured by this camera is similar to how a normal human perceives the world but with several simplifications. Due to its simplicity, it is widely adopted in computer graphics industry.

### Thin Lens Camera

![Thin Lens Camera Teaser](Example/thinlens_camera_teaser.jpg "Image rendered by a thin-lens camera. Notice the depth of field effect.")

For thin lens camera, as its name suggests, the lens system in this camera is assumed to be a single lens with negligible thickness. The biggest advantage of it is that depth of field effects are possible under this model. In the following render, depth of field is achieved by a 72 mm lens focusing on the first metallic monkey.

### Environment Camera

### Energy Measurement

## Image

### Texture

### Film

### Filtering

### Tone-mapping

### Reading and Writing

### Merging

## Material

### Surface Behavior

### Volume Behavior

## Geometry

### Intersectable and Primitive

### Basic Shapes

### Advanced Shapes

### Acceleration Structure

## Light

### Emitter

## Sample Source

### Sample Generator





