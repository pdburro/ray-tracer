# Ray Tracer

A C++ ray tracer for COMS 3360, following the *Ray Tracing* book series.

## Book progress

**Completed:** [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html), through Section 14.

**Current:** [Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html), Section 4: Texture Mapping — complete.

**Next:** Book 2, Section 5: Perlin Noise.

## Course checkpoint

- [x] Camera with configurable position, orientation, and field of view
- [x] Anti-aliasing
- [x] Ray/sphere intersections
- [ ] Ray/triangle intersections
- [x] Load textures from image files (JPEG, PNG, and BMP verified with stb_image)
- [ ] Textured spheres and triangles (spheres complete; triangles still needed)
- [ ] Load and render triangle meshes
- [x] Spatial subdivision acceleration structure (BVH)
- [x] Specular, diffuse, and dielectric materials
- [ ] Emissive materials (lights)

**Additional features:** Depth of field and motion blur. Small diffuse spheres move during a shutter interval of [0, 1); stationary spheres stay fixed.

## Build and render

Requires CMake 3.16+ and a C++17 compiler. From PowerShell, with Visual Studio Build Tools installed:

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\ray_tracer.exe | Out-File -Encoding ascii renders\image.ppm
```

The default scene is Earth with an image texture. Other scenes and custom image files can be selected like this:

```powershell
.\build\Release\ray_tracer.exe checkers | Out-File -Encoding ascii renders\checkers.ppm
.\build\Release\ray_tracer.exe spheres | Out-File -Encoding ascii renders\spheres.ppm
.\build\Release\ray_tracer.exe earth "path\to\texture.png" | Out-File -Encoding ascii renders\custom.ppm
```

Open the result with a viewer that supports PPM. Render settings are in `src/main.cpp`: 400x225 pixels, 100 samples per pixel for Earth/checkers (25 for spheres), and a maximum path depth of 50. CMake copies the bundled textures beside the executable. Build files and generated renders are ignored by Git.

Texture mapping supports constant colors, 3D checker patterns, and image textures on spheres using UV coordinates. All seven test suites pass; the Earth, checker, and moving-sphere renders have been checked. A missing or unreadable image reports an error.

To run the tests:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

## Credits

Based on *Ray Tracing in One Weekend* and *Ray Tracing: The Next Week* by Peter Shirley, Trevor David Black, and Steve Hollasch. Implemented with Codex assistance.

Image loading uses [stb_image](external/README.md). The Earth texture comes from the textbook; see [asset sources](assets/README.md).
