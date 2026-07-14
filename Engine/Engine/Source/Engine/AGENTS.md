# Engine Guide

## Film and Observable Metadata
- `EFilm` defines each film output's meaning; do not add a parallel semantic enum when film type
  already determines the contract.
- `FilmSetting` owns configured layer metadata. Film implementations accumulate and develop
  samples; they do not own or report working color space metadata.
- `RenderObservableInfo` contains client-visible layer, progress, and statistic descriptions.
  `Engine` adds configured `FilmSetting` layer names to renderer-provided progress and statistic
  descriptions.
- Renderer frame APIs return native layer data. Engine frame APIs form the client boundary: convert
  radiance layers to linear sRGB and leave numeric layers unchanged.
