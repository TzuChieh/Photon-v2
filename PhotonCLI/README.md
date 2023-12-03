# Command-line Interface (CLI)

Photon-v2 comes with an application called `PhotonCLI`, which is a command-line interface of the render engine. Command-line interface can come in handy if you are batch rendering or using it on a remote server. It also, in theory, offers slightly better performance in terms of render time. This is a documentation of available options and some examples. Check out `PhotonStudio` if you are more interested in using a GUI.

## Available Options

| Options  | Effects |
| -------- | ------- |
| `-s <path>` | Specify path to scene file. To render an image series, you can specify `"myScene*.p2"` as `<path>` where `*` is a wildcard for any string (`--series` is required in this case). (default path: `"./scene.p2"`) |
| `-o <path>` | Specify image output path. This should be a filename for single image and a directory for image series. (default path: `"./rendered_scene.png"`) |
| `-of <format>` | Specify the format of output image. Supported formats are: `png`, `jpg`, `bmp`, `tga`, `hdr`, `exr`. If this option is omitted, format is deduced from filename extension. |
| `-t <number>` | Set number of threads used for rendering. (default: `1`, single thread) |
| `-p <number>` | Output an intermediate image whenever the render has progressed `<number>`%. (default: never output intermediate image) |
| `--raw` | Do not perform any post-processing. (default: perform post-processing) |
| `--help` | Print help message then exit. |
| `--series` | Render an image series. The order for rendering will be lexicographical order of the wildcarded string. Currently only .png is supported. |
| `--start <*>` | Render image series starting from a specific wildcarded string. |
| `--finish <*>` | Render image series until a specific wildcarded string is matched. (inclusive) |

## Examples

Rendering a scene file named `./ocean.p2` using 4 threads and save the rendered image as `./my_image.jpg`:

```shell
PhotonCLI -s "./ocean.p2" -o "./my_image.jpg" -t 4
```

For animations, assuming there is a folder `gif_animation` containing scene files for a 3-frame animation: `pose1.p2`, `pose2.p2`, `pose3.p2`, to render and save the images (.png) to a folder `gif_images`, you can specify:

```shell
PhotonCLI -s "./gif_animation/pose*.p2" --series --start "1" --finish "3" -o "./gif_images/" -t 4
```

Note that in the above example, the rendered image will be named by the wildcarded string, namely they are stored as `./gif_images/1.png`, `./gif_images/2.png`, and `./gif_images/3.png`.

An important note is that by default, post effects including tone-mapping is performed. If saving the rendered image in high dynamic range is desired, `--raw` should be specified manually as `PhotonCLI` will *NOT* automatically turn off post-processing by detecting image format.
