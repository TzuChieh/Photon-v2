# Feature Overview

[TOC]

A quick overview of the features supported by Photon. Similar to [Project Structure](./Documentation/project_structure.md), this page can also serve as a handy reference for you to find the documentation or source code that you need.

## Visualizer

Rendering is the process of visualizing a scene. In Photon, we use a [visualizer](@ref ph::Visualizer) to generate a [renderer](@ref ph::Renderer) to actually render an image. This is where most of the core algorithms meet and work together--and is arguably the most complex part of a renderer. While there are many types of renderers to choose from, there is hardly an all-purpose renderer as each one of them is designed to deal with different situations. The efficiency of a rendering algorithm is also partly depending on the characteristics of a scene, e.g., sizes of geometries, material properties, etc. At the extreme end, one can always construct a scene that will fail a certain rendering algorithm. Be sure to understand and try different renderers if things do not work out.

### Backward Unidirectional Path Tracing

![BVPT Teaser](055_books_based_on_Libri_sulla_mensola_books_by_archemi.jpg "An example image rendered using this renderer.")

wip wip wip

### Scheduler

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


### Acceleration Structure

## Light

### Emitter

## Sample Source

### Sample Generator





