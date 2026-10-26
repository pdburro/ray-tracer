# Ray Tracer

A C++ class project following [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) by Peter Shirley, Trevor David Black, and Steve Hollasch.

## Current status

Section 8 implemented: the camera averages 100 randomly jittered rays per pixel to smooth sphere and ground edges. The scene remains 400x225 with surface-normal colors and a sky background. `samples_per_pixel` is configurable in `src/main.cpp`; camera position/orientation and field of view remain fixed until Section 12. Next: Section 9, diffuse scattering and gamma correction.

## Project layout

```text
src/main.cpp     Scene construction and camera settings
src/camera.h     Camera setup, jittered rays, sample averaging, and render loop
src/random.h     Reproducible random values in [0, 1)
src/interval.h   Valid ray parameter ranges
src/hittable.h   Hit records and abstract intersection interface
src/sphere.h     Sphere roots and oriented surface normals
src/hittable_list.h  Nearest-hit scene traversal
tests/intersections.cpp  Intersection regression checks
src/ray.h        Ray origin, direction, and at(t)
src/vec3.h       Vector math and point3 alias
src/color.h      Color alias and PPM pixel output
CMakeLists.txt   C++ build configuration
renders/        Generated images (ignored by Git)
```

Add headers under `src/` as you reach the corresponding concepts in the guide.

## Build and run

Requirements: CMake 3.16 or newer and a C++17 compiler. On Windows, Visual Studio Build Tools with the Desktop development with C++ workload is one option.

From this folder in PowerShell:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

With a Visual Studio generator, run:

```powershell
.\build\Release\ray_tracer.exe
```

With a single-configuration generator, the executable is usually `build/ray_tracer.exe` on Windows or `build/ray_tracer` on macOS/Linux. Configure those builds with `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`.

Save a render in PowerShell using:

```powershell
.\build\Release\ray_tracer.exe | Out-File -Encoding ascii renders\image.ppm
```

Keep progress messages on `std::clog` so they do not enter the image file. Use a viewer that supports PPM to inspect the result.

## Checkpoints

Check off each milestone after building, running, and inspecting its result. These follow the guide's progression:

- [x] 00 — Create the repository scaffold and build configuration.
- [x] 01 — Build and run the starter locally.
- [x] 02 — Output an Image: save and inspect the PPM gradient; add progress reporting.
- [x] 03 — The vec3 Class: add vector operations and color output.
- [x] 04 — Rays, a Simple Camera, and Background: render a sky gradient.
- [x] 05 — Adding a Sphere: render a sphere silhouette.
- [x] 06 — Surface Normals and Multiple Objects: visualize normals and add a ground sphere.
- [x] 07 — Moving Camera Code Into Its Own Class: preserve the rendered scene.
- [x] 08 — Antialiasing: compare sampled edges with the earlier image.
- [ ] 09 — Diffuse Materials: add diffuse scattering and gamma correction.
- [ ] 10 — Metal: add reflective and fuzzy materials.
- [ ] 11 — Dielectrics: add glass and refraction.
- [ ] 12 — Positionable Camera: render the scene from a new viewpoint.
- [ ] 13 — Defocus Blur: demonstrate depth of field.
- [ ] 14 — Where Next?: render the final scene and record settings.

## First course checkpoint requirements

This course checkpoint spans multiple textbook sections and additional features. Sections 6-8 complete the sphere intersection foundation, camera organization, and antialiasing.

- [ ] Camera with configurable position, orientation, and field of view (Sections 7 and 12).
- [x] Anti-aliasing (Section 8).
- [x] Ray/sphere intersections (Sections 5-6).
- [ ] Ray/triangle intersections (extension).
- [ ] Load image textures (extension; PPM is a possible initial format).
- [ ] Textured spheres and triangles with UV coordinates (extension).
- [ ] Load and render triangle meshes (extension; OBJ is a possible initial format).
- [ ] Spatial subdivision acceleration structure (extension; BVH is an option).
- [ ] Specular, diffuse, and dielectric materials (Sections 9-11).
- [ ] Emissive materials / lights (extension).

Texture, BVH, and emissive concepts are covered in the series' second volume; triangles and mesh loading require additional implementation. Defocus blur is optional for this course checklist.

## Verification

```powershell
ctest --test-dir build -C Release --output-on-failure
```

The intersection test covers non-unit directions, both sphere roots, interval endpoints, rays starting inside a sphere, oriented normals, tangents, zero directions, empty scenes, and nearest-hit selection independent of insertion order. Section 6 render outputs are `renders/section06.ppm` and a PNG preview at `renders/section06.png` (both ignored by Git).

Section 7 output is `renders/section07.ppm`, verified byte-for-byte identical to Section 6 (all 90,000 pixels).

### Section 7 learning notes

- `main.cpp` owns scene construction and the public camera settings (`image_width`, `aspect_ratio`).
- `camera::render(world)` initializes the camera and generates one center ray per pixel, then writes its color.
- Private `initialize()` derives image height, viewport dimensions, pixel spacing, and the first pixel center from those settings before each render.
- Private `ray_color()` retains the existing surface-normal colors and sky background.
- This is a refactor: geometry and pixel colors stay the same. Camera position, orientation, and field of view arrive in Section 12; antialiasing is next in Section 8.

### Section 8 learning notes

A pixel represents an area of the image. Sampling only its center makes object boundaries jump between whole pixels. `get_ray(i, j)` now samples a random location within that area using offsets in [-0.5, 0.5) along each pixel axis. The camera sums the resulting colors and multiplies by `1.0 / samples_per_pixel` before writing the pixel. More samples reduce sampling noise and increase rendering work roughly in proportion to the sample count.

- Set `cam.samples_per_pixel = 100` in `src/main.cpp` (9,000,000 primary rays at 400x225). The class default is 10; values below 1 are rejected.
- One sample now means one random location, rather than the old center sample. Section 7's saved render is the center-sampled comparison.
- `random_double()` uses a fixed initial seed, allowing repeatable runs with the same toolchain. The generator advances across renders in one process.
- Averaging happens in floating point before converting to integer RGB. Gamma correction comes in Section 9.
- Outputs: `renders/section08.ppm`, `renders/section08.png`, and `renders/section08-comparison.png` (Section 7 left, Section 8 right; same crop enlarged 6x). Generated images remain ignored by Git.
- Verification: Release build and CTest passed; checked all 90,000 pixels and RGB ranges. At 810 boundary pixels, RGB root-mean-square error against a separate uniform 16x16-sample reference fell from 25.204 for center sampling to 3.662 for 100 random samples. Visually inspected the enlarged comparison.

## Version control workflow

After finishing a checkpoint, update its checkbox and record what you verified in the log below. Review changes before staging them:

```powershell
git status
git diff
git add README.md src CMakeLists.txt .gitignore renders/.gitkeep
git commit -m "Complete checkpoint 02: first PPM image"
git push -u origin main
```

Use a commit message matching the checkpoint you actually completed. After the first push, `git push` is enough. Optionally mark a verified checkpoint with `git tag checkpoint-02` and publish that tag with `git push origin checkpoint-02`.

Generated images and build files are ignored. To preserve a milestone image, deliberately copy it into a tracked `docs/images/` folder and add it to your commit.

## Progress log

| Checkpoint | Date | Verification / notes |
| --- | --- | --- |
| 00 | 2026-09-10 | Starter files added; compilation has not yet been verified. |
| 01-03 | 2026-09-29 | Release build succeeded; generated a 256x256 rainbow PPM and checked pixel count and integer channel ranges. Added Section 3 vector/color helpers and progress reporting. |
| 04 | 2026-10-20 | Release build passed; checked 400x225 PPM pixel count, channel ranges, and blue-to-white direction. |

| 05 | 2026-10-20 | Release build passed; verified all 90,000 pixels against the projected sphere silhouette, including 13,254 red pixels, and checked PPM channel ranges. |

| 06 | 2026-10-26 | Release build and CTest passed. Rendered 400x225 at one ray per pixel; independently checked all 90,000 pixels (13,254 foreground sphere, 33,894 ground, 42,852 sky), channel ranges, and visually inspected the PNG preview. |

| 07 | 2026-10-26 | Release build and CTest passed. Moved camera/render code to camera.h; 400x225 PPM is byte-for-byte identical to Section 6. |

| 08 | 2026-10-26 | Release build and CTest passed; rendered 400x225 at 100 samples/pixel. Validated PPM and inspected enlarged edges; boundary RGB RMSE against a 16x16 reference improved from 25.204 to 3.662. |

For future entries, record image settings, what changed, and any remaining issues.

## Reference and attribution

The linked book is the learning reference for this project. The interfaces and math in src/vec3.h and src/color.h follow Section 3, implemented with Codex assistance. The ray class and simple camera/background follow Section 4 with Codex assistance. The sphere intersection follows Section 5 with Codex assistance. The interval, hittable interface, hit records, oriented normals, sphere root selection, and object list follow Section 6 with Codex assistance. The camera class organization follows Section 7 with Codex assistance. Pixel jitter, sample accumulation, and averaging follow Section 8 with Codex assistance; the random helper uses the C++ standard library generator. Credit any adapted code as it is added, and follow your class's rules for outside code and assistance.
