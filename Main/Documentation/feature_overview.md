# Feature Overview

[TOC]

A quick overview of the features supported by Photon. Similar to [Project Structure](./Documentation/project_structure.md), this page can also serve as a handy reference for you to find the documentation or source code that you need.

## Visualizer

Rendering is the process of visualizing a scene. In Photon, we use a [Visualizer](@ref ph::Visualizer) to generate a [Renderer](@ref ph::Renderer) to actually render an image. This is where most of the core algorithms meet and work together--and is arguably the most complex part of a renderer. While there are many types of renderers to choose from, there is hardly an all-purpose renderer as each one of them is designed to deal with different situations. The efficiency of a rendering algorithm is also partly depending on the characteristics of a scene, e.g., sizes of geometries, material properties, etc. At the extreme end, one can always construct a scene that will fail a certain rendering algorithm. Be sure to understand and try different renderers if things do not work out.

### Unidirectional Path Tracing

![BVPT Teaser](Gallery/055_books_based_on_Libri_sulla_mensola_books_by_archemi.jpg "An example image rendered using this renderer.")

Though this is a relatively simple renderer to solve the *rendering equation* as proposed by Kajiya @cite Kajiya:1986:Rendering, this renderer is a good starting point before diving into other solvers. Backward unidirectional path tracing does not handle small light sources well, and scenes with hard-to-find light transport paths are likely to fail, too. This renderer can be constructed by creating a [PathTracingVisualizer](@ref ph::PathTracingVisualizer) with [BVPT](@ref ph::ERayEnergyEstimator::BVPT) energy estimator (use [BVPTDL](@ref ph::ERayEnergyEstimator::BVPTDL) if only direct lighting is required).

### Unidirectional Path Tracing with NEE

![BNEEPT Teaser](Gallery/bathroom_based_on_Salle_de_bain_by_nacimus.jpg "NEE can handle more complex scenes.")

With the addition of Next Event Estimation (NEE), unidirectional path tracing can handle common scenes quite effectively. While largely based on the same principle as the non-NEE variant, this renderer combines additional sampling techniques @cite Veach:1995:Optimally to better handle smaller light sources. Similarly, create a [PathTracingVisualizer](@ref ph::PathTracingVisualizer) with [BNEEPT](@ref ph::ERayEnergyEstimator::BNEEPT) energy estimator to use this renderer.

### Vanilla Photon Mapping

![VPM Teaser](Example/vpm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 500M photons (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

It is worth to clarify that the project is named *Photon* not because the focus is on photon mapping techniques, but because photon is the elementary particle that transmit light, and the core of this project is to solve light transport problems. Nevertheless, several photon mapping techniques are available here, and the most fundamental one is vanilla photon mapping @cite Jensen:1996:Global. All photon mapping techniques are capable of *merging* nearby light transport paths in some way, and this form of path reuse allows them to excel on a wider variety of lighting phenomena. You can get a vanilla photon mapping renderer by creating a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Vanilla](@ref ph::EPhotonMappingMode::Vanilla) mode.

### Progressive Photon Mapping

![PPM Teaser](Example/ppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

This renderer implements the PPM algorithm by Hachisuka et al. @cite Hachisuka:2008:Progressive The first pass traces a fixed set of view vertices then the following iterative process will estimate the incoming energy on those vertices. The algorithm can leverage effectively infinite amount of photons to render a scene, achieving superior results then vanilla photon mapping. Create a [PhotonMappingVisualizer](@ref ph::PhotonMappingVisualizer) in [Progressive](@ref ph::EPhotonMappingMode::Progressive) mode to use this renderer.

### Stochastic Progressive Photon Mapping

![SPPM Teaser](Example/sppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

### Probabilistic Progressive Photon Mapping

![PPPM Teaser](Example/pppm_perfumes_by_gp3991.jpg "Perfume bottle and its cap rendered with 0.5M photons for 1000 passes (radius: 0.1, spp: 4). The image has not rendered to convergence to better show the characteristics between different photon mapping techniques.")

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

### Receiver

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





