# Film Merger {#photon_film_merger_readme}

This is a tool for combining render output from multiple render sessions. A typical use case is the same scene being
rendered by multiple machines, each producing a different render output. We call these render outputs "films". Multiple films can be merged into a single film in various ways. For example, statistically independent films can be merged into a more converged film with reduced noise and bias.

> [!note]
> If you are building the engine from source, you may need to enable the CMake option `PH_BUILD_OPTIONAL_TOOLS` to build this tool.

## Available Options

Primary usage:

```shell
[<film-path> [-w <weight>]]... [-o <image-path>] [-of <image-format>] [--help]
```

See below for more information about the arguments.

| Options  | Effects |
| -------- | ------- |
| `<film-path>` | File path of the film to merge. |
| `-w <weight>` | Specify a weighting factor for the film. If the film format does not come with weighting factors, 1 will be used. |
| `-o <image-path>` | Specify output path for the merged image. This should be a filename without the extension part. Note that the application will not create the directory for you if it is not already exists. (default path: `"./merged_image"`) |
| `-of <image-format>` | Specify the format of the merged image. Supported formats are: png, jpg, bmp, tga, hdr, exr, pfm. (default format: exr) |
| `--help` | Print this help message then exit. |

## Examples

All following commands are expected to be executed from the build/install directory.

To merge three films into a new one named `"./merged.exr"`, use the following command:

```shell
./bin/FilmMerger "./film1.exr" "./film2.exr" "./film3.exr" -o "./merged"
```

You can also specify a weighting factor for each film. In the following example, two films are merged with a 1:2 ratio:

```shell
./bin/FilmMerger "./film1.exr" -w 1 "./film2.exr" -w 2
```

The film merger tool accepts an arbitrary number of films.
