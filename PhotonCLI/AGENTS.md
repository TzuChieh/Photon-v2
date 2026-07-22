# PhotonCLI Guide

## Engine Integration
- PhotonCLI receives engine SDL and resource support through CEngine and `phInit()`; engine SDL
  additions need no CLI-side registration, so modify PhotonCLI only for command-line behavior.

## Output Naming
- Treat `-o` values as image stem overrides by output index. Empty or omitted override slots use the default image output stem.
- In `--series` mode, `-o` is the output directory; each frame uses the extracted wildcard text as the image stem under that directory.
