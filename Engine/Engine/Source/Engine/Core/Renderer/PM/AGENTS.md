# Photon Mapping Guide

## SPPM Details
- SPPM `num-samples-per-pixel` is converted to `floor(sqrt(value))` statistics records per pixel dimension. Non-square values silently lose intent.
- SPPM view-path tracing samples raster coordinates from the statistics sampling grid, then maps each sample to a statistics record by `floor(statisticsRes * rasterSample)`.
- SPPM final film writes use deterministic statistics-record centers via `statisticsRegion.xy01ToSurface((record + 0.5) / statisticsRes)`.
- SPPM statistics represent finite raster regions. The current film write path uses `addSample()`, which the code comment says is exact for box filtering and otherwise may overblur; boundaries are limited by not storing out-of-raster-bound statistics.
- For tests, `ZTestVerifier(sample_count=...)` should use progressive pass count, not SPPM statistics count.
- `min-photon-path-length = 2` prevents storing photons at photon path length 1, i.e. the first surface hit after emission.
