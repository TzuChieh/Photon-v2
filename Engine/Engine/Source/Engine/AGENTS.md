# Engine Guide

## SDL Metadata
- For SDL-reflected fields, keep `defaultTo()` or `noDefault()` authoritative; do not repeat
  `defaultTo()` values in owner member initializers.
- For SDL-reflected types, do not add default constructors solely to reproduce SDL defaults; use
  `TSdl<T>::make()` or `makeResource()` when an instance must be initialized from SDL metadata.
- Keep SDL-facing type and field names and descriptions implementation-neutral and extension-oriented;
  put external-renderer compatibility claims and terminology in concrete implementation comments.

## External Algorithms
- For rewritten external algorithms, keep original-author/source provenance and immutable
  path/function permalinks beside the corresponding implementation; do not cite unused helpers.

## Film and Observable Metadata
- `EFilm` defines each film output's meaning; do not add a parallel semantic enum when film type
  already determines the contract.
- `FilmSetting` owns numeric/color semantics and renderer output color space; do not duplicate that
  metadata in film implementations.
- `RenderObservableInfo` contains client-visible layer, progress, and statistic descriptions.
  `Engine` adds configured `FilmSetting` layer names to renderer-provided progress and statistic
  descriptions.
- Renderer frame APIs return each layer in its declared renderer output color space. Engine frame
  APIs convert color layers to linear sRGB before optional post-processing and leave numeric layers
  unchanged.
