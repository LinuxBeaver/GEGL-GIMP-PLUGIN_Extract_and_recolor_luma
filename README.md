# GEGL Extract and Color Luminance Channel
GEGL Extracts the brightness channel from an image and colorizes and blurs it. This filter was inspired by Gimp/GEGL's "Bloom" and Pixelitor's "Extract channel" filter. It shows that GEGL has the ability to extract channels for non-destructive editing with blend modes.  

### Rules for filter to work

1. Filter requires an alpha channel

2. It is highly recommended you use the blend mode addition. 

3.Gimp's layer blend modes will support an external glow effect on transparency. GEGL's layer blend modes will NOT support an external glow effect on transparency.

Here is a preview of me using the filter with Gimp's layer blend modes correctly to get an external glow effect.
![image preview](preview_of_EL.png  )



## Compiling and Installing

### Linux

To compile and install you will need the GEGL header files (`libgegl-dev` on
Debian based distributions or `gegl` on Arch Linux) and meson (`meson` on
most distributions).

```bash
meson setup --buildtype=release build
ninja -C build
cp build/high-pass-box.so ~/.local/share/gegl-0.4/plug-ins
```

If you have an older version of gegl you may need to copy to `~/.local/share/gegl-0.3/plug-ins`
instead (on Ubuntu 18.04 for example).



### Windows

The easiest way to compile this project on Windows is by using msys2.  Download
and install it from here: https://www.msys2.org/

Open a msys2 terminal with `C:\msys64\mingw64.exe`.  Run the following to
install required build dependencies:

```bash
pacman --noconfirm -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-meson mingw-w64-x86_64-gegl
```

Then build the same way you would on Linux:

```bash
meson setup --buildtype=release build
ninja -C build
```

