# Ray Tracer

A C++ class project following [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) by Peter Shirley, Trevor David Black, and Steve Hollasch.

## Current status

Project scaffold created. The executable currently prints a startup message; rendering is the first implementation checkpoint. Build and render checkpoints stay unchecked until verified locally.

## Project layout

```text
src/main.cpp     Program entry point
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

Once you implement PPM output on standard output, save a render in PowerShell using:

```powershell
.\build\Release\ray_tracer.exe | Out-File -Encoding ascii renders\image.ppm
```

Keep progress messages on `std::clog` so they do not enter the image file. Use a viewer that supports PPM to inspect the result.

## Checkpoints

Check off each milestone after building, running, and inspecting its result. These follow the guide's progression:

- [x] 00 — Create the repository scaffold and build configuration.
- [ ] 01 — Build and run the starter locally.
- [ ] 02 — Output an Image: save and inspect the PPM gradient; add progress reporting.
- [ ] 03 — The vec3 Class: add vector operations and color output.
- [ ] 04 — Rays, a Simple Camera, and Background: render a sky gradient.
- [ ] 05 — Adding a Sphere: render a sphere silhouette.
- [ ] 06 — Surface Normals and Multiple Objects: visualize normals and add a ground sphere.
- [ ] 07 — Moving Camera Code Into Its Own Class: preserve the rendered scene.
- [ ] 08 — Antialiasing: compare sampled edges with the earlier image.
- [ ] 09 — Diffuse Materials: add diffuse scattering and gamma correction.
- [ ] 10 — Metal: add reflective and fuzzy materials.
- [ ] 11 — Dielectrics: add glass and refraction.
- [ ] 12 — Positionable Camera: render the scene from a new viewpoint.
- [ ] 13 — Defocus Blur: demonstrate depth of field.
- [ ] 14 — Where Next?: render the final scene and record settings.

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

For future entries, record image settings, what changed, and any remaining issues.

## Reference and attribution

The linked book is the learning reference for this project. Credit any adapted code as it is added, and follow your class's rules for outside code and assistance.
