# Ray Tracer

A C++ ray tracer for COMS 3360, following the *Ray Tracing* book series.

## Book progress

**Completed:** [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html), through Section 14.

**Next:** [Ray Tracing: The Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html), Motion Blur. Book 2 has not been started.

## Course checkpoint

- [x] Camera with configurable position, orientation, and field of view
- [x] Anti-aliasing
- [x] Ray/sphere intersections
- [ ] Ray/triangle intersections
- [ ] Load textures from image files
- [ ] Textured spheres and triangles
- [ ] Load and render triangle meshes
- [ ] Spatial subdivision acceleration structure
- [x] Specular, diffuse, and dielectric materials
- [ ] Emissive materials (lights)

**Additional feature:** Depth of field, with configurable focus distance and defocus angle.

## Build and render

Requires CMake 3.16+ and a C++17 compiler. From PowerShell, with Visual Studio Build Tools installed:

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\ray_tracer.exe | Out-File -Encoding ascii renders\image.ppm
```

Open the result with a viewer that supports PPM. Render settings are in `src/main.cpp`: currently 400x225 pixels, 25 samples per pixel, and a maximum path depth of 50. Build files and generated renders are ignored by Git.

To run the tests:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

## Credits

Based on *Ray Tracing in One Weekend* by Peter Shirley, Trevor David Black, and Steve Hollasch. Implemented with Codex assistance.
