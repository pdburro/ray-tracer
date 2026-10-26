# Ray Tracer

A C++ ray tracer for COMS 3360, following the *Ray Tracing* book series.

## Book progress

**Completed:** [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html), through Section 14.

**Current:** [Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html), Section 3: Bounding Volume Hierarchies — complete.

**Next:** Book 2, Section 4: Texture Mapping.

## Course checkpoint

- [x] Camera with configurable position, orientation, and field of view
- [x] Anti-aliasing
- [x] Ray/sphere intersections
- [ ] Ray/triangle intersections
- [ ] Load textures from image files
- [ ] Textured spheres and triangles
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

Open the result with a viewer that supports PPM. Render settings are in `src/main.cpp`: currently 400x225 pixels, 25 samples per pixel, and a maximum path depth of 50. Build files and generated renders are ignored by Git.

The scene now uses a BVH to skip groups of objects a ray cannot reach. The Section 3 render matches Section 2 byte-for-byte and took about 6.2 seconds, compared with 26.8 seconds before acceleration (individual runs on this machine). All six test suites pass.

To run the tests:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

## Credits

Based on *Ray Tracing in One Weekend* and *Ray Tracing: The Next Week* by Peter Shirley, Trevor David Black, and Steve Hollasch. Implemented with Codex assistance.
