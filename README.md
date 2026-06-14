# COMP3011 Computer Graphics
Repository for all Computer Graphics work.

## Lab Setup
Typical VisualStudio2022 project setup.

1. specify the include directory

`../../include;`

2. specify the library directory

`../../lib/glfw-3.4.bin.WIN64/lib-vc2022;`

3. add `stb_image.h` to your pre-processor defines.

`STB_IMAGE_IMPLEMENTATION;`

4. add the glfw3.lib in the additional dependency in the linker input.

`glfw3.lib;`

5. add `gl3w.c` to the source files in the project.

6. add utility `.h` files to projects header files

## Coursework Image Compare Tool
ImageCompare verify the correctness of rendered images.

from `.\COMP3011\ImageCompare\x64\Debug\`
```bash
.\ImageCompare.exe ..\..\..\Assessment1A\Assessment1A\render.bmp rasterise.bmp
```

from `.\COMP3011`
```bash
.\ImageCompare\x64\Debug\ImageCompare.exe .\Assessment1A\Assessment1A\render.bmp .\ImageCompare\x64\Debug\rasterise.bmp
```
