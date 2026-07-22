# Photon Mapping Guide

## SPPM Details
- Treat SPPM `num-samples-per-pixel` as a square statistics-grid request; reject or make rounding
  explicit rather than assuming arbitrary values are preserved.
- Model SPPM statistics as finite subpixel regions; keep collection and reconstruction mappings
  consistent, and make non-box filtering explicitly average regions and handle raster boundaries.
- Do not use path-tracing `ZTestVerifier` variance estimates for biased methods like photon-mapping since they require a different way to estimate variance; use deterministic, analytic, or visual checks suited to the case.
