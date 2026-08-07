# Engine Guide

## SDL Metadata
- Prefer documenting SDL-reflected types in their `PH_DEFINE_SDL_CLASS()` metadata instead of
  duplicating the description in C++ Doxygen comments.
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
- Let `EFilm` define output meaning and `FilmSetting` own numeric/color semantics and renderer
  output color space; do not add parallel enums or duplicate this metadata in film implementations.
- `RenderObservableInfo` contains client-visible layer, progress, and statistic descriptions.
  `Engine` adds configured `FilmSetting` layer names to renderer-provided progress and statistic
  descriptions.
- Renderer frame APIs return each layer in its declared renderer output color space. Engine frame
  APIs convert color layers to linear sRGB before optional post-processing and leave numeric layers
  unchanged.
