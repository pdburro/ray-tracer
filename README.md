# Ray Tracer

A C++ class project following [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html) by Peter Shirley, Trevor David Black, and Steve Hollasch.

## Current status

Section 14 implemented: the first volume's final random-sphere scene combines diffuse, metal, glass, antialiasing, configurable camera pose/FOV, and depth of field. The current fixed-seed scene contains 484 objects: 480 small spheres, three large feature spheres, and ground. Development render settings are 400x225, 25 samples per pixel, and depth 50. The first volume is complete; the remaining course requirements are triangles, image textures, textured spheres/triangles, mesh loading, acceleration, and emissive materials.

## Project layout

```text
src/main.cpp     Final random-sphere scene construction and camera settings
src/camera.h     Camera pose/FOV, focus/lens sampling, jittered rays, and rendering
src/random.h     Reproducible random values in [0, 1) or a specified range
src/interval.h   Valid ray parameter ranges
src/hittable.h   Hit records and abstract intersection interface
src/sphere.h     Sphere intersections, oriented normals, assigned material
src/material.h   Scatter interface, Lambertian diffuse, metal, and dielectric glass
src/hittable_list.h  Nearest-hit scene traversal
tests/intersections.cpp  Intersection regression checks
tests/diffuse.cpp  Sampling, recursion, and gamma regression checks
tests/materials.cpp  Reflection, refraction, attenuation, fuzz, and boundary checks
tests/camera.cpp  Camera pose/FOV, lens sampling, focus-plane rays, and invalid settings
src/ray.h        Ray origin, direction, and at(t)
src/vec3.h       Vector math, disk/direction sampling, reflection, and refraction
src/color.h      Color alias, gamma encoding, clamped PPM pixel output
CMakeLists.txt   C++ build configuration
renders/        Generated images (ignored by Git)
```

Add headers under `src/` as you reach the corresponding concepts in the guide.

## Reading the code alongside the book

Source comments refer to **Book 1, Ray Tracing in One Weekend**. A section comment above a class or group of related helpers applies to that block; local comments identify later additions. Comments marked "Project" describe our validation, diagnostics, or regression tests rather than textbook listings.

| Book section | Main implementation |
| --- | --- |
| 2: Output an Image | `camera::render`, `write_color` |
| 3: The vec3 Class | `vec3.h`, the `point3`/`color` aliases |
| 4: Rays, a Simple Camera, and Background | `ray.h`, the sky portion of `camera::ray_color` |
| 5-6: Spheres, Normals, and Multiple Objects | `sphere::hit`, `hit_record`, `hittable`, `hittable_list`, `interval` |
| 7: Camera Class | `camera::render`, `camera::initialize` |
| 8: Antialiasing | `random_double`, `sample_square`, `get_ray`, sample averaging |
| 9: Diffuse Materials | `random_unit_vector`, `lambertian_direction`, recursive `ray_color`, gamma encoding |
| 10: Metal | `material`, `lambertian`, `metal`, `reflect`, material references in hits |
| 11: Dielectrics | `dielectric`, `refract`, Schlick reflectance |
| 12: Positionable Camera | Camera pose/FOV settings and the u/v/w frame in `initialize` |
| 13: Defocus Blur | `random_in_unit_disk`, lens disk setup, `defocus_disk_sample`, `get_ray` |
| 14: Final Scene | `main.cpp`, ranged random values, random colors |

The tests in `tests/` are project checks, with comments naming the textbook features they exercise. The 2026-10-26 readability review added concise section references and explanations without changing executable code. Release build and all four CTest suites passed afterward.

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
- [x] 09 — Diffuse Materials: add diffuse scattering and gamma correction.
- [x] 10 — Metal: add reflective and fuzzy materials.
- [x] 11 — Dielectrics: add glass and refraction.
- [x] 12 — Positionable Camera: render the scene from a new viewpoint.
- [x] 13 — Defocus Blur: demonstrate depth of field.
- [x] 14 — Where Next?: render the final scene and record settings.

## First course checkpoint requirements

This course checkpoint spans multiple textbook sections and additional features. Sections 6-12 complete sphere intersections, configurable camera position/orientation/FOV, antialiasing, and diffuse, specular, and dielectric materials. Triangles, textures, meshes, acceleration, and emissive materials remain.

- [x] Camera with configurable position, orientation, and field of view (Sections 7 and 12).
- [x] Anti-aliasing (Section 8).
- [x] Ray/sphere intersections (Sections 5-6).
- [ ] Ray/triangle intersections (extension).
- [ ] Load image textures (extension; PPM is a possible initial format).
- [ ] Textured spheres and triangles with UV coordinates (extension).
- [ ] Load and render triangle meshes (extension; OBJ is a possible initial format).
- [ ] Spatial subdivision acceleration structure (extension; BVH is an option).
- [x] Specular, diffuse, and dielectric materials (Sections 9-11).
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

### Section 9 learning notes

**What changed:** Surface-normal debug colors have been replaced by light gathered along randomly scattered paths. The blue-to-white sky is the light source; there are no emissive objects yet. Both spheres currently share fixed gray diffuse behavior. Separate material objects arrive in Section 10.

1. **Random directions (`vec3.h`):** Generate a point with each coordinate in [-1, 1), reject points outside the unit sphere or with squared length at most `1e-160`, and divide accepted points by their length. Rejection avoids the directional bias of directly normalizing random cube points; the lower limit avoids division by zero or underflow.
2. **Lambertian scattering (`vec3.h`):** The book first explores uniform hemisphere scattering, then uses `normal + random_unit_vector()` to favor directions near the surface normal. We implement that final cosine-weighted distribution. If the sum is nearly zero, use the normal instead. Ray directions need not have unit length for the sphere intersection formula.
3. **Recursive light gathering (`camera.h`):** On a hit, launch a ray from the hit point along the sampled direction. Return `0.5 * ray_color(scattered_ray, depth - 1, world)`. On a miss, return the sky color. Thus one surface retains half the incoming contribution, two retain one quarter, and so on. This traces from camera toward the environment to estimate light reaching the camera.
4. **Bounce limit (`camera.h`, `main.cpp`):** Begin with `cam.max_depth = 50`; depth zero returns black. This bounds work and recursion. Truncating a path discards its remaining contribution. The class default is 10, negative values are rejected, and zero intentionally produces black.
5. **Shadow acne (`camera.h`):** Use an intersection interval starting at `t = 0.001`. Floating-point error can otherwise make a scattered ray immediately hit the surface it just left. This is the book's simple scene-scale-dependent tolerance, not a universal world-space distance because directions are not normalized.
6. **Gamma and output (`color.h`):** Continue averaging samples in linear space. Only when writing the final pixel, use `sqrt(component)` for positive values and zero for nonpositive ones. This approximates gamma-2 encoding, rather than the exact sRGB transfer function. For example, linear 0.25 encodes as 0.5 (byte 128). Clamp encoded values to [0, 0.999] before multiplying by 256, keeping RGB bytes in [0, 255]. Encoding each sample before averaging would give the wrong result.

**Reading the result:** The surfaces look matte and slightly blue from the sky. The contact region is dark because paths there tend to encounter more surfaces and lose more light. Grain is finite-sample noise; increasing `samples_per_pixel` reduces it at increased rendering cost. Antialiasing and diffuse scattering now share the same random helper.

**Verification:** Release build and both CTest suites passed. New tests check 20,000 unit-vector and Lambertian samples, outward nonzero directions, directional bias, mean cosine near 2/3 (the cosine-weighted distribution), recursion termination with an always-hit synthetic scene, the 0.001 hit tolerance, gamma conversion, and byte clamping. Validated all 90,000 rendered pixels and channel ranges, then visually inspected `renders/section09.png`. The render uses 400x225, 100 samples per pixel, and depth 50. Outputs are `renders/section09.ppm` and `renders/section09.png`, ignored by Git.

### Section 10 learning notes

**Separating shape from material:** Geometry answers where a ray hits; a material answers what happens there. `material.h` defines an abstract `scatter(incoming, record, attenuation, scattered)` interface. A true result supplies a new ray and RGB attenuation; false means absorption. `lambertian` and `metal` implement that interface, allowing the camera to trace either without material-specific branches.

**Connecting objects and materials:** Each sphere requires a non-null `shared_ptr<material>` and copies it into the hit record when intersected. Shared pointers let several objects reuse one material and keep it alive while referenced. `hittable.h` forward-declares `material` because a pointer does not require its full definition; `material.h` includes the hit record definition. This avoids circular header dependencies.

**Diffuse albedo:** `lambertian` retains Section 9's `lambertian_direction(normal)` sampling, including the near-zero fallback. Its attenuation is now a per-material color instead of fixed gray 0.5. For example, blue albedo (0.1, 0.2, 0.5) retains 10% of red, 20% of green, and 50% of blue at each bounce. The camera uses component-wise multiplication: `attenuation * ray_color(scattered, depth - 1, world)`.

**Reflection math:** For incoming direction v and unit normal n, `reflect(v, n) = v - 2 * dot(v, n) * n`. The dot product measures the normal component. Subtracting twice that component reverses it while preserving the component parallel to the surface and the vector length. Example: (1, -1, 0) reflects to (1, 1, 0) at normal (0, 1, 0). This gives equal incident and reflection angles.

**Fuzzy metal:** Normalize the incoming direction, reflect it, then add `fuzz * random_unit_vector()`. Normalizing before adding fuzz makes roughness independent of the original ray length. Fuzz 0 gives an exact mirror direction; larger fuzz blurs the reflected scene. Clamp fuzz to [0, 1] (we also clamp negatives, extending the book's upper clamp). Accept a scattered ray only if its dot product with the normal is positive; otherwise return false and the camera contributes black. This is the book's simple rough-reflection model, not a full physical microfacet metal model.

**Scene:** Ground: Lambertian (0.8, 0.8, 0.0), center at (0, -100.5, -1), radius 100. Center: Lambertian (0.1, 0.2, 0.5), center (0, 0, -1.2), radius 0.5. Left: metal (0.8, 0.8, 0.8), fuzz 0.3, center (-1, 0, -1), radius 0.5. Right: metal (0.8, 0.6, 0.2), fuzz 1.0, center (1, 0, -1), radius 0.5. Silver shows recognizable reflected surroundings; gold is much blurrier. Both metal appearances are tinted by their RGB attenuation. Sky illumination, antialiasing, depth limits, intersection tolerance, and gamma encoding continue from earlier sections.

**Verification:** Release build and all three CTest suites passed. Added checks for exact zero-fuzz reflection, length preservation, invariance to incoming ray length, scatter origins and albedo, lower/upper fuzz clamping, and both accepted and absorbed grazing bounces. Existing intersection tests now verify material propagation; diffuse recursion tests now use an assigned Lambertian material. Rejected null sphere materials are also tested. Validated all 90,000 output pixels and byte ranges, and visually inspected `renders/section10.png`. Settings: 400x225, 100 samples per pixel, maximum depth 50. PPM and PNG outputs in `renders/` remain ignored by Git.

### Section 11 learning notes

**What changes:** `dielectric` implements the existing material interface, so the camera and sphere intersection code need no changes. `refract()` in `vec3.h` supplies the new direction math. Glass has attenuation (1, 1, 1): this is ideal clear, nonabsorbing glass with no tint or wavelength-dependent dispersion.

1. **Snell's law:** `n_in * sin(theta_in) = n_out * sin(theta_out)`. Air to glass uses ratio `n_in / n_out = 1 / 1.5`; glass to air uses 1.5. Higher-index glass bends transmitted light toward the normal on entry and away on exit. Angles are measured from the normal. At normal incidence the transmitted direction is unchanged.
2. **Vector refraction:** Normalize the incoming direction u, compute `cos_theta = dot(-u, normal)`, and scale the surface-tangent component: `perpendicular = ratio * (u + cos_theta * normal)`. Complete a unit outgoing direction with `parallel = -sqrt(abs(1 - perpendicular.length_squared())) * normal`. The result is their sum. The absolute value protects against tiny rounding errors; it does not make impossible refraction valid, so the caller checks for total reflection first.
3. **Entering or leaving:** The constructor stores the object's refractive index divided by that of the enclosing medium. A front-face hit uses its reciprocal for the incoming/outgoing ratio; a back-face hit uses the stored value. The existing hit normal always opposes the incident ray. Constructor indices must be positive and finite.
4. **Total internal reflection:** Compute `sin_theta = sqrt(1 - cos_theta * cos_theta)`. If `ratio * sin_theta > 1`, no transmitted angle exists and the ray must reflect. This occurs when crossing from higher to lower index at sufficiently grazing incidence. For glass (1.5) to air (1), the critical angle is about 41.8 degrees. An air bubble in a denser enclosing medium can also exhibit this at a front-face hit.
5. **Schlick approximation:** `R0 = ((1 - ratio) / (1 + ratio))^2`, then `R = R0 + (1 - R0) * (1 - cos_theta)^5`. Normal-incidence air/glass reflectance is about 4%; grazing reflectance approaches 100%. When transmission is possible, compare R with a random value in [0, 1) to choose one reflected or refracted ray. We do not multiply by R again: choosing the branch with that probability already supplies its weight across samples. One child ray per interaction avoids exponential path branching.
6. **Hollow glass:** The left outer sphere at (-1, 0, -1) has radius 0.5 and dielectric index 1.5. Its concentric air cavity has positive radius 0.4 and relative index `1 / 1.5`. A transmitted path can cross air -> glass -> cavity air -> glass -> outside air. This follows the current textbook's relative-index approach; no negative-radius sphere is needed. These explicitly assigned relative indices describe the known nesting; there is no general automatic medium stack.

**Scene and output:** The blue diffuse center and yellow diffuse ground remain. The right gold metal now uses fuzz 0 to match the book's final Section 11 scene. The hollow sphere transmits a distorted view and reflects its surroundings, especially near the rim. Outputs: `renders/section11.ppm` and `renders/section11.png` (ignored by Git). Settings remain 400x225, 100 samples per pixel, depth 50. Some sampling grain remains.

**Verification:** Release build and all three CTest suites passed. Added checks for Snell refraction into/out of glass, unit transmitted directions, unchanged normal-incidence transmission, total internal reflection, clear attenuation and hit origin, correct front/back index ratios, and normalization of non-unit input rays. Repeated samples check reflection frequency near 4% at normal incidence and increasing reflectance at grazing incidence. Tests trace a straight central ray through all four hollow-sphere boundaries to verify distances, front/back flags, and material identity. Invalid refractive indices are rejected. All 90,000 rendered pixels and RGB ranges were validated, then the PNG was visually inspected.

### Section 12 learning notes

**Public settings (`camera.h`, configured in `main.cpp`):**

| Setting | Meaning | Current scene | Default |
| --- | --- | --- | --- |
| `lookfrom` | Camera position in world coordinates | (-2, 2, 1) | (0, 0, 0) |
| `lookat` | Target point along the view direction | (0, 0, -1) | (0, 0, -1) |
| `vup` | Reference up direction, controlling roll | (0, 1, 0) | (0, 1, 0) |
| `vfov` | Vertical field of view in degrees | 20 | 90 |

**Field of view:** Convert degrees to radians: `theta = vfov * pi / 180`. The viewport is placed distance d = `length(lookfrom - lookat)` from the camera. A right triangle with half-angle theta/2 gives `viewport_height = 2 * d * tan(theta / 2)`. Its width is the height multiplied by the actual image aspect ratio (`image_width / image_height`). Narrowing the angle magnifies the view without changing perspective; moving the camera changes perspective. This remains a pinhole camera, so d does not yet introduce depth-of-field blur.

**Camera axes:** `w = unit_vector(lookfrom - lookat)` points backward, opposite the viewing direction. `u = unit_vector(cross(vup, w))` points right. `v = cross(w, u)` points up. These perpendicular unit axes form an orthonormal camera frame. `vup` need not be perpendicular to the view direction or have unit length: we normalize it before the cross product, and the resulting v is the perpendicular camera-relative up. Changing vup can roll the image.

**Viewport and rays:** Replace fixed world X/Y viewport vectors with `viewport_width * u` and `-viewport_height * v`. The negative sign advances image rows downward. The upper-left corner is `lookfrom - d * w - viewport_u / 2 - viewport_v / 2`; add half a pixel step in each direction to locate the first pixel center. Existing jitter then selects a location within each pixel. Every ray starts at lookfrom and points toward that sampled location. `initialize()` rebuilds all derived geometry at the start of each render, allowing settings to change between renders.

**Input checks:** Reject nonfinite vectors/FOV, FOV outside (0, 180), coincident or extremely close position/target, zero or parallel up vectors, and invalid image dimensions/aspect ratios before writing output. Degenerate vectors cannot form a valid camera basis. These checks extend the textbook implementation with explicit errors.

**Scene and verification:** Objects and materials remain unchanged from Section 11, including polished gold metal. The elevated diagonal viewpoint and narrow FOV give a close view, with the glass sphere partly outside the frame. Release build and all four CTest suites passed. Camera tests capture emitted rays and verify origins, jittered pixel bounds, 20/60/90-degree FOV, rotation to face +X, roll, non-unit up, and setting changes between renders. They also confirm that increasing target distance along the same axis preserves angular bounds, and invalid configurations fail before ray generation. Validated all 90,000 PPM pixels and RGB ranges and visually inspected `renders/section12.png`. Outputs: `renders/section12.ppm` and `renders/section12.png` (ignored by Git); 400x225, 100 samples per pixel, max depth 50.

### Section 13 learning notes

**Why blur appears:** A pinhole camera emits every primary ray from one point. A finite lens instead admits rays across an area. Our thin-lens approximation sends rays from a sampled circular lens toward the sampled pixel on a focus plane. They converge at that plane; nearer/farther surfaces intersect different parts of the ray bundle and become blurred when samples are averaged. This is depth-dependent defocus, not a blur filter applied to the finished image.

**New controls (`camera.h`, configured in `main.cpp`):** `focus_dist` is the axial distance from the camera to the plane of perfect focus (current 3.4, class default 10). `defocus_angle` is the full angle subtended by the lens disk at the focus-plane center (current 10 degrees, default 0). The lens radius is `focus_dist * tan(defocus_angle * pi / 360)`. Larger angles increase blur; zero uses exactly the pinhole origin. We reject negative, nonfinite, or >=180-degree angles and nonpositive/nonfinite focus distances. This explicitly rejects negative angles rather than treating them as disabled.

**Independent focus and direction:** `lookat` now only establishes the viewing direction with `lookfrom`. The viewport height and placement use focus_dist instead of the distance to lookat. The height remains `2 * focus_dist * tan(vfov / 2)`, and its center is `lookfrom - focus_dist * w`. Scaling viewport dimensions and placement together preserves angular framing. This is a simplified thin-lens model; focus distance is not the physical focal length of a real camera.

**Disk sampling (`vec3.h`):** Generate x/y uniformly in [-1, 1), set z=0, and reject samples with squared radius >=1. The accepted samples are uniform over disk area. Sampling only a circle's rim or using a uniform radius would give the wrong distribution. Unlike the 3D unit-direction sampler, disk points are not normalized.

**Ray construction (`camera.h`):** Multiply the camera's right/up unit vectors by the lens radius to get `defocus_disk_u/v`. For disk sample p, the origin is `lookfrom + p.x() * defocus_disk_u + p.y() * defocus_disk_v`. Select the pixel's jittered point on the focus plane as before, then set direction to `pixel_sample - origin`. At zero defocus angle, bypass disk sampling and use lookfrom. Lens sampling and pixel jitter solve different problems: depth of field and antialiasing. Random scattering, attenuation, depth limits, and gamma encoding remain unchanged.

**Result and settings:** Objects/materials and camera pose match Section 12. With focus_dist 3.4 and defocus_angle 10, the blue sphere is relatively sharp, while the foreground glass and background gold sphere visibly blur. More samples reduce the remaining grain, but do not remove the intentional depth blur. Settings: 400x225, 100 samples per pixel, max depth 50, vertical FOV 20 degrees. Outputs: `renders/section13.ppm`, `renders/section13.png`, and `renders/section13-comparison.png` (Section 12 pinhole left, Section 13 defocus right); generated images remain ignored by Git.

**Verification:** Release build and all four CTest suites passed. Camera tests now intersect captured rays with the focus plane and check that each lands inside its intended jittered pixel region. They verify origins within the correctly oriented lens disk, varied origins at positive aperture, multiple focus distances, and pinhole origins after disabling blur. A 20,000-sample disk check verifies bounds, near-zero mean, and mean squared radius near 1/2, as expected for uniform disk area. Added invalid focus/aperture checks. Validated all 90,000 output pixels and RGB ranges and visually inspected the side-by-side comparison.

### Section 14 learning notes

**Purpose:** This section adds a complex scene using the renderer built in Sections 2-13. It does not add a new scattering model. Scene construction is in `src/main.cpp`; the new ranged `random_double(min, max)` and `vec3::random(min, max)` helpers support random positions and material colors. Random values are evaluated in explicit order so construction does not depend on C++ argument evaluation order.

**Scene generation:** A gray diffuse sphere centered at (0, -1000, 0), radius 1000, forms the ground. Iterate a 22x22 grid with a and b from -11 through 10. Each candidate small sphere has center `(a + 0.9 * random, 0.2, b + 0.9 * random)` and radius 0.2, placing it on the ground near its grid cell. Skip candidates within 0.9 of (4, 0.2, 0) to leave room for the large metal sphere, following the book. This is not a general sphere packing or overlap prevention algorithm.

**Materials:** Choose diffuse with probability 0.8, metal with probability 0.15, and glass with probability 0.05. Diffuse albedo is the component-wise product of two random RGB colors, favoring darker colors. Metal albedo channels lie in [0.5, 1), with fuzz in [0, 0.5). Glass uses index 1.5. Add three radius-1 spheres: glass at (0, 1, 0), brown diffuse at (-4, 1, 0) with albedo (0.4, 0.2, 0.1), and polished metal at (4, 1, 0) with albedo (0.7, 0.6, 0.5). These are solid spheres; this final scene replaces the earlier hollow-glass demonstration.

**Reproducibility:** The random generator begins with seed 5489 on each fresh program run. On the verified toolchain the scene has 378 small diffuse spheres, 79 small metal spheres, and 23 small glass spheres: 480 small spheres plus three large spheres and ground = 484 objects. Scene counts are written to stderr through `std::clog`, keeping PPM stdout clean. Standard-library distribution implementations can differ across toolchains, so exact layouts are not promised across platforms.

**Camera:** Position (13, 2, 3), target (0, 0, 0), up (0, 1, 0), vertical FOV 20 degrees, defocus angle 0.6 degrees, focus distance 10, and maximum depth 50. This gives a wide arrangement of spheres with a narrow viewing angle, subtle depth blur, reflections, and refracted surroundings.

**Render quality:** The verified development settings are width 400, height 225, and 25 samples per pixel (2,250,000 primary paths, plus scattered rays). For the book's high-quality render, change `cam.image_width` to 1200 and `cam.samples_per_pixel` to 500 in `src/main.cpp`; height becomes 675. That is 180 times as many primary samples, before differences in path work. The current scene still checks every object per ray, so render cost is substantial and grain remains at development quality. A BVH acceleration structure is a remaining course requirement.

**Verification:** Release build and all four CTest suites passed. The development render exited successfully in about 30.6 seconds on this machine. Validated all 90,000 PPM pixels, RGB byte ranges, and nontrivial image content, then visually inspected the PNG for the sphere arrangement, diffuse surfaces, metal reflections, refracted surroundings, and defocus blur. Outputs: `renders/section14.ppm` and `renders/section14.png`, ignored by Git.

**Course work still outstanding:** Ray/triangle intersections; loading image textures and applying UVs to spheres and triangles; loading and rendering triangle meshes; a spatial acceleration structure such as a BVH; and emissive materials/lights. Completing the first volume does not complete this course checkpoint. The second volume supplies texture, BVH, and light concepts; triangles and mesh I/O need additional implementation.

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
| 09 | 2026-10-26 | Release build and both CTest suites passed. Lambertian scene at 400x225, 100 samples/pixel, depth 50; validated 90,000 pixels and visually inspected gamma-corrected PNG. |
| 10 | 2026-10-26 | Release build and all three CTest suites passed. Four-sphere diffuse/fuzzy-metal scene at 400x225, 100 samples/pixel, depth 50; validated 90,000 pixels and visually inspected PNG. |
| 11 | 2026-10-26 | Release build and all three CTest suites passed. Hollow glass scene at 400x225, 100 samples/pixel, depth 50; tested refraction, total reflection, Schlick probabilities, and four cavity boundaries; validated pixels and inspected PNG. |
| 12 | 2026-10-26 | Release build and all four CTest suites passed. Camera at (-2,2,1), target (0,0,-1), up (0,1,0), vfov 20; 400x225, 100 samples/pixel, depth 50. Validated ray geometry, output pixels, and PNG appearance. |
| 13 | 2026-10-26 | Release build and all four CTest suites passed. Thin lens at focus_dist 3.4, defocus_angle 10 degrees; 400x225, 100 samples/pixel, depth 50. Tested lens distribution and focus-plane intersections; validated output and inspected comparison. |

| 14 | 2026-10-26 | Release build and all four CTest suites passed. Final scene: 484 objects, 400x225, 25 samples/pixel, depth 50; rendered in about 30.6 seconds. Validated all 90,000 pixels and visually inspected PNG. First volume complete; course extensions remain. |

| Code review | 2026-10-26 | Reviewed all source and test files; added Book 1 section references and concise math comments. Confirmed code text unchanged after excluding comments/whitespace; Release build and all four CTest suites passed. |

For future entries, record image settings, what changed, and any remaining issues.

## Reference and attribution

The linked book is the learning reference for this project. The interfaces and math in src/vec3.h and src/color.h follow Section 3, implemented with Codex assistance. The ray class and simple camera/background follow Section 4 with Codex assistance. The sphere intersection follows Section 5 with Codex assistance. The interval, hittable interface, hit records, oriented normals, sphere root selection, and object list follow Section 6 with Codex assistance. The camera class organization follows Section 7 with Codex assistance. Pixel jitter, sample accumulation, and averaging follow Section 8 with Codex assistance; the random helper uses the C++ standard library generator. Random direction rejection sampling, Lambertian scattering, recursive attenuation, depth limits, intersection tolerance, and gamma-2 conversion follow Section 9 with Codex assistance. The material interface, per-object albedo, mirror reflection, and fuzzy metal model follow Section 10 with Codex assistance. Snell refraction, total internal reflection, Schlick reflectance, and the relative-index hollow glass scene follow Section 11 with Codex assistance. Viewport FOV geometry and camera frame construction follow Section 12 with Codex assistance. Thin-lens geometry, disk sampling, focus distance, and defocus angle follow Section 13 with Codex assistance. The random-sphere scene, material probabilities, feature spheres, and final camera settings follow Section 14 with Codex assistance; development resolution and sample count are reduced from the high-quality book settings. Credit any adapted code as it is added, and follow your class's rules for outside code and assistance.
