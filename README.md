# Toy RayTracer: A Playground of Light

[![C++ CI Build](https://img.shields.io/github/actions/workflow/status/pacyu/ray-tracer/cmake-build.yml?branch=master&label=build&passed_color=7b9e87&failed_color=bd2c00&style=flat-square)](https://github.com/pacyu/ray-tracer/actions)
[![License](https://img.shields.io/badge/License-GPLv3-7b9e87?style=flat-square)](https://www.gnu.org/licenses/gpl-3.0)
[![Language](https://img.shields.io/badge/Language-C++-5a738e?style=flat-square)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-5a738e?style=flat-square)](https://github.com/pacyu/ray-tracer)
[![OpenCV](https://img.shields.io/badge/Dependencies-OpenCV-6b8eac?style=flat-square&logo=opencv&logoColor=white)](https://opencv.org/)
[![OpenMP](https://img.shields.io/badge/Parallel-OpenMP-6b8eac?style=flat-square&logo=cpu-fill&logoColor=white)](https://www.openmp.org/)
[![GitHub code size](https://img.shields.io/github/languages/code-size/pacyu/ray-tracer?color=8c92ac&style=flat-square)](https://github.com/pacyu/ray-tracer)

[中文](#chinese) | [English](#english) | [日本語](#japanese)

---

![sponze's model](/examples/test_sponze.png)
![Car's model](/examples/test_car.png)
![Water material](/examples/test_ocean.png)
![Dynamics ocean](/examples/output.gif)
![Taubin's heart](/examples/image.png)

---

## Latest Updates (2026.06.14)

- **修复了许多问题**:
  1. obj 解析器的已知问题修复，不过有些属性暂时没有利用，比如 `s、o、g`，解析了他们，但没有使用，尤其是 `o、g` 的名字，暂时忽略他们的用处，`s` 可能有些用，用于标记一些面共享顶点？但暂时没有处理。
  2. 修复了 Mesh 的各种问题，尤其是因为 `pdf_value` 和 `random` 导致的问题，所以修改了所有 `hittable` 的相关接口。
  3. 将 Mesh 内部的 BVH 构建算法升级为了 SAH 算法。
  4. 修复 `Cloth` 材质无法正确渲染，纯黑色的原因，因为原先的写法会导致 srec.pdf_ptr 在 scattering_pdf 中变成悬空指针。
  5. 修复了 `Plastic`、`Glossy Plastic` 的黑噪点问题。
  6. 修复了背景贴图方向问题，背景贴图应该无关于相机 Up 方向，所以矫正为无论是 Y-Up 还是 Z-Up 永远都是正确的方向。
  7. 修复 Vec2 `+` 重载计算错误，第二个分量计算不小心写成了 `*`，导致 `Mesh` 的纹理计算老是有问题，现已修复。
- **已知问题**:
  1. `Dielectric` 材质应用在 `Box` 会失效，失效主要是指颜色会失效，无论如何设置颜色、密度、折射率，Box 都是透明无色的，排查过所有问题：每个面的法向量设置、hit 反转法向量、光线步进 delta、射线检测的容差精度等等，全都测试过，依然无效，暂时无法解决。
  2. `sponza` 场景虽然能渲染，但仍存在有一些问题，比如墙的纹理不太对、存在一些白噪点。

`Datsun_280z` 渲染信息:

```bash
成功加载HDR: ../textures/autumn_field_puresky_4k.hdr [4096x2048]
正在解析材质: Datsun_280z.mtl
材质解析完成!
正在加载模型: ../models/free-datsun-280z/Datsun_280Z.obj
模型加载完成!
模型顶点个数: 320460, 面数量: 106820
[Build] 开始构建 BVH ...
[Build] BVH 构建完毕！用时: 0 s
渲染进度: 100.00% | 已用时间: 9.5s | 预计剩余: 0.0s  
```

`sponza` 渲染信息:

```bash
成功加载HDR: ../textures/autumn_field_puresky_4k.hdr [4096x2048]
正在解析材质: sponza.mtl
材质解析完成!
正在加载模型: ../models/sponza/sponza.obj
成功加载纹理: ../models/sponza/textures\sponza_thorn_diff.png [512x512]
成功加载纹理: ../models/sponza/textures\sponza_thorn_diff.png [512x512]
成功加载纹理: ../models/sponza/textures\sponza_thorn_ddn.png [256x256]
成功加载纹理: ../models/sponza/textures\sponza_thorn_mask.png [512x512]
成功加载纹理: ../models/sponza/textures\vase_round.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_round.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_round_ddn.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_plant.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_plant.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_plant_NRM.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_plant_mask.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_plant.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_plant.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_plant_mask.png [1024x1024]
成功加载纹理: ../models/sponza/textures\background.png [1024x1024]
成功加载纹理: ../models/sponza/textures\background.png [1024x1024]
成功加载纹理: ../models/sponza/textures\background_ddn.png [1024x1024]
成功加载纹理: ../models/sponza/textures\gi_flag.png [128x128]
成功加载纹理: ../models/sponza/textures\gi_flag.png [128x128]
成功加载纹理: ../models/sponza/textures\spnza_bricks_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\spnza_bricks_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\spnza_bricks_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_arch_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_arch_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_ceiling_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_ceiling_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_a_ddn.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_floor_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_floor_a_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_c_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_c_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_c_ddn.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_details_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_details_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_b_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_b_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_column_b_ddn.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_flagpole_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_flagpole_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_fabric_green_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_fabric_green_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_fabric_blue_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_fabric_blue_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_fabric_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_fabric_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_curtain_blue_diff.png [2048x2048]
成功加载纹理: ../models/sponza/textures\sponza_curtain_blue_diff.png [2048x2048]
成功加载纹理: ../models/sponza/textures\sponza_curtain_diff.png [2048x2048]
成功加载纹理: ../models/sponza/textures\sponza_curtain_diff.png [2048x2048]
成功加载纹理: ../models/sponza/textures\sponza_curtain_green_diff.png [2048x2048]
成功加载纹理: ../models/sponza/textures\sponza_curtain_green_diff.png [2048x2048]
成功加载纹理: ../models/sponza/textures\chain_texture.png [256x1024]
成功加载纹理: ../models/sponza/textures\chain_texture.png [256x1024]
成功加载纹理: ../models/sponza/textures\chain_texture_ddn.png [256x1024]
成功加载纹理: ../models/sponza/textures\chain_texture_mask.png [256x1024]
成功加载纹理: ../models/sponza/textures\vase_hanging.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_hanging.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_dif.png [1024x1024]
成功加载纹理: ../models/sponza/textures\vase_dif.png [1024x1024]
成功加载纹理: ../models/sponza/textures\lion.png [1024x1024]
成功加载纹理: ../models/sponza/textures\lion.png [1024x1024]
成功加载纹理: ../models/sponza/textures\lion_ddn.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_roof_diff.png [1024x1024]
成功加载纹理: ../models/sponza/textures\sponza_roof_diff.png [1024x1024]
模型加载完成!
模型顶点个数: 786828, 面数量: 262276
[Build] 开始构建 BVH ...
Sponza Min: (-1920.95, -126.442, -1182.81)
Sponza Max: (1799.91, 1429.43, 1105.43)
[Build] BVH 构建完毕！用时: 0 s
渲染进度: 100.00% | 已用时间: 658.8s | 预计剩余: 0.0s 
```

## Updates (2026.06.09)

- **性能优化**: 为了提升Mesh模型的构建性能和渲染性能，重构了 `Mesh`，使用更紧凑的 `BVH` 结构进行加速，并且只存储索引，这样无论是速度还是内存占用都大大减少。

`Datsun_280Z.obj`渲染信息（修复: 该数量不正确，我发现是因为最初没完全理清 `obj` 格式，导致解析器重复添加了多遍模型的所有数据导致的，不过，虽然不对，就当做压力测试了，该问题已修复）：

```bash
成功加载HDR: ../textures/autumn_field_puresky_4k.hdr [4096x2048]
正在解析材质: Datsun_D280z.mtl
材质解析完成!
正在加载模型: ../models/free-datsun-280z/Datsun_280Z.obj
模型加载完成!
模型顶点个数: 4169892, 面数量: 1389964
[Build] 开始构建 BVH ...
[Build] BVH 构建完毕！用时: 0 s
渲染进度: 96.67% | 已用时间: 27.9s | 预计剩余: 1.0s
```

## Updates (2026.06.06)

- **obj 模型支持**: 手写实现了一个 `.obj` 解析器（还未经过充分测试，不确定 `mtl` 材质是否能完全正确渲染），现在渲染器支持 `.obj` 模型加载并渲染了。
- **一些优化**: 为了支持通用的模型，重构了下 `Triangle`、`Mesh`、`Ocean`。

`Datsun_280Z.obj` 渲染信息:

```bash
成功加载HDR: ../textures/autumn_field_puresky_4k.hdr [4096x2048]
正在解析材质: Datsun_D280z.mtl
材质解析完成!
正在加载模型: ../models/free-datsun-280z/Datsun_280Z.obj
模型加载完成!
模型顶点个数: 4169892, 面数量: 1389964
[Build] 开始构建 BVH ...
[Build] BVH 构建完毕！用时: 47 s
渲染进度: 89.33% | 已用时间: 117s | 预计剩余: 0.1s
```

## Updates (2026.04.02)

- **项目重构**: 对各个部件进行了分类，现在的项目目录结构更便于管理材质、纹理、几何对象。
- **新增材质**: 增加 `Plastic`、`OceanMaterial`（材质名已改为 Water）、`GlossyPlastic`、`Cloth` 材质。
- **新增海洋可渲染对象**: `Ocean` 作为一个几何物体，通过 Phillips Spectrum([J Tessendorf · 2004](https://jtessen.people.clemson.edu/reports/papers_files/coursenotes2004.pdf)) 可以通过给定分辨率、风速、风向等创建。

## Updates (2026.03.25)

- **Auroric 语法重构**: 完全重构了解释器后端，现在 `aur` 脚本写起来会更灵活，自然。代码示例参考[这里](/bin/scene.aur)。
- **性能优化**: 增加了 `BVH` 树进行场景管理以及命中判断加速，对于简单场景性能提升比较少，也做了一部分智能指针优化，但性能提升不明显。
- **新增纹理**: 增加 `ImageTexture`（图片纹理），支持图片作为纹理的渲染效果。

## Updates (2026.03.16)

- **Auroric 语法重构**：全面支持变量引用、`=` 赋值表达式及材质/物体定义。
- **物理渲染增强**：引入 **Beer's Law**（比尔定律），支持高质量彩色玻璃渲染。
- **隐式曲面修复**：彻底解决 `Heart` 方程的表面裂纹问题，并修复了轴向旋转 (RotateX/Y/Z) 逻辑。
- **新增对象**：增加 `ConstantMedium`（参与介质），支持烟雾、体积雾效果。

## Updates (2026.03.10)

本次重大更新引入了全新的场景描述语言 —— **`Auroric` (.aur)**。用户可以通过编写 `.aur` 脚本动态配置物体、材质、相机和光源，彻底摆脱了修改场景必须重新编译代码的烦恼。

---

<a name="chinese"></a>

## 🇨🇳 中文 (Chinese)

### 关于项目

**本项目有一个宗旨：如果可能，为了保持项目的整洁，尽可能的手写对各种依赖的实现，尽量不依赖第三方库。除非性能问题。**

这是一个基于物理的光线追踪渲染器（RayTracer），起步于学习图形学的兴趣。该项目最初源自经典的“一周光线追踪”系列教程，后面会考虑增加物理引擎。目的是通过该项目学习计算机图形学、数学（辛几何、PDE、数值算法等）、语言（英语和日语）、物理学（广义相对论/天体/宇宙物理/量子物理等相关知识），以此来了解世界底层原理以及拓展知识，便于接触前沿学术。目前是利用CPU多线程加速渲染，只能做到离线渲染，后面会迁移到GPU，利用CUDA性能加速。

### 核心特性

* **Auroric 脚本语言**：
  * 支持变量赋值 (`x = Num 10`) 与引用。
  * 材质与物体解耦定义。
  * 智能初始化：内置默认 `Camera` 配置，开箱即画。
* **基于物理渲染 (PBR)**：
  * 支持漫反射、金属、电介质材质。
  * **彩色玻璃**：通过 Beer's Law 实现光线在介质中的吸收效果。
* **数学与几何**：
  * **隐式曲面**：完美修复的心形 (Heart) 方程，支持平滑的 RotateX/Y/Z 变换。
  * **体积渲染**：支持 `ConstantMedium` 对象，用于模拟各向同性的散射介质。
* **性能优化**：集成 OpenMP 多线程加速，利用 OpenCV 进行图像后期处理。
* **重要性采样 (IS)**：通过采样权重优化，有效减少画面噪点。
* **多线程加速**：集成 **OpenMP**，充分压榨 CPU 核心性能。
* **图像存储**：利用 **OpenCV** 处理并保存高质量图像。

### 未来蓝图

* [x] **场景配置语言**：自定义 DSL，无需重新编译即可动态定义物体、光源和相机。
* [x] **跨平台支持**：支持 Linux 和 Windows。
* [x] **Auroric 扩展**: `Random` 随机数生成。
* [ ] **Auroric 进阶**：为 Auroric 增加 `for` 循环支持。
* [x] **几何体扩展**：增加三角面片（Mesh）和更复杂的 SDF 物体。
* [x] **第三方模型导入**: 增加支持 .obj 等格式的模型导入渲染功能。
* [x] **海洋模拟**
* [ ] **非均匀密度材质**
* [ ] **黎曼几何/光线弯曲**
* [ ] **流体模拟**
* [ ] **三体问题模拟**
* [ ] **卡通渲染**

---

<a name="english"></a>

## 🇺🇸 English

### About

A physically-based RayTracer built from scratch as a hobbyist project. Originally inspired by the "Ray Tracing in One Weekend" series, it is now evolving into a flexible rendering tool with advanced features.

### Key Features

* **Auroric DSL (.aur)**: A custom-designed, human-readable configuration language.
  * Now supports variables, `=` expressions, and material/object decoupling. Default Camera auto-initialization.
* **PBR Workflow**:
  * Supports Lambertian, Metal, and Dielectric (Glass) materials.
  * Implementation of **Beer's Law** for realistic tinted glass rendering.
* **Mathematical Shapes**:
  * Native support for implicit surfaces (e.g., the 3D Heart Equation).
  * Added `ConstantMedium` for fog and smoke effects.
* **Importance Sampling**: Optimized light path calculation for faster noise reduction.
* **Parallel Rendering**: Powered by **OpenMP** for high-performance CPU rendering.
* **Image Output**: Integrated with **OpenCV** for image handling and storage.

### Roadmap

* [x] **Scene Configuration DSL**: A custom language to define scenes/objects without recompilation.
* [x] **Cross-Platform Support**: Migration to CMake for Linux and Windows support.
* [x] **Advanced Geometry**: Mesh loading (.obj) and complex SDF shapes.

---

<a name="japanese"></a>

## 🇯🇵 日本語 (Japanese)

### プロジェクトについて

これは物理ベースのレイトレーシング・レンダラーです。最初は学習用の小さなプロジェクトとして始まりましたが、現在は機能拡張を続ける趣味のレンダリングツールへと進化しています。

### 主な機能

* **Auroric DSL (.aur)**: シーン構成を直感的に記述できる独自言語。
  * 変数定義、代入式、材質・物体の定義をサポート。デフォルトカメラによる自動初期化。
* **物理ベースレンダリング (PBR)**:
  * 拡散反射、金属、ガラス（誘電体）などの質感を再現。
  * **Beer's Law** (ベールの法則) による彩色ガラスの実装。
* **数学的な形状**:
  * 隠関数による複雑な曲面（ハート方程式など）をサポート。
  * **ボリュームレンダリング**: `ConstantMedium` による霧や煙の表現が可能。
* **重要性サンプリング**: ノイズを効率的に抑えるための最適化アルゴリズムを搭載。
* **並列化**: **OpenMP** によるマルチスレッド高速レンダリング。
* **画像処理**: **OpenCV** を使用したレンダリング結果の保存。

### ロードマップ

* [x] **シーン設定言語**: コードを再コンパイルせずにシーンや物体を定義できる独自言語の実装。
* [x] **マルチプラットフォーム**: Linux および Windows への対応。
* [x] **高度なジオメトリ**: 三角メッシュの読み込みと SDF 形状の追加。

---

## Build & Setup (Windows/Linux)

### 1. Prerequisites

* **Compiler**: Visual Studio 2017+ (or GCC/Clang with C++17 support).
* **CMake**: Version 3.10 or higher.
* **[OpenCV](https://opencv.org/)**: Ensure OpenCV is installed. The default search path is `C:/OpenCV/build`. (Please update `OpenCV_DIR` in `CMakeLists.txt` if your path is different).

### 2. Compilation

```bash
# Clone the repository
git clone https://github.com/pacyu/ray-tracer.git
cd ray-tracer

# Create build directory
mkdir build
cd build

# Configure and Build
cmake ..
cmake --build . --config Release
```

### 3. Running a Scene

```bash
cd bin
./rayt scene.aur
```

### 4. Auroric Script Example

```aur
# Example .aur scene file

spp = 128

texture = String "C:/Users/darkchii/Downloads/img.png"

red = Lambert ((0.65, 0.05, 0.05))
green = Lambert ((0.12, 0.45, 0.15))
grey = Lambert ((0.73, 0.73, 0.73))
blue = Metal ((0.06, 0.32, 0.73), 0.0)
glass = Dielect ((0.6953, 0.7422, 0.7070), 0.08, 1.5)
img_texture = Lambert (texture)

light = Light ((7, 7, 7))

rect_light = XYRect ((-100, 100, -100, 100), 277.0, light)

quad = Quater (-278, 278, -278, 278)

floor = XYRect (quad, -278.0, grey)

ceiling = XYRect (quad, 278.0, grey)

inner = YZRect (quad, -278.0, grey)

right = XZRect (quad, 278.0, red)

left = XZRect (quad, -278.0, green)

box = Box ((100, -150, -277), (200, -50, -50), blue, (0, 0, 45.0))

sphere = Sphere ((100, 100, -200), 80.0, glass)

img_sphere = Sphere ((120, 10, -80), 100.0, img_texture)

boll = Sphere ((-100, 0, -200), 80.0, glass)

cm = ConstMedium ((0.5294, 0.8078, 0.9216), 0.05, boll)

heart = Heart ((0, 0, 0), 120.0, red, (0, 0, 90))

world = [rect_light, floor, ceiling, inner, right, left, box, sphere, cm, img_sphere, heart]
```

## Credits

Originally based on Ray Tracing in One Weekend. Auroric DSL parser implemented with a custom recursive descent approach.

## Acknowledgements

This project legal framework complies with the GNU GPL v3.0. It integrates and builds upon the following high-performance libraries:

- [FFTW3](https://www.fftw.org/) (GPL-2.0-or-later) - Powering the ocean wave FFT simulations.
- [OpenCV](https://opencv.org/) (Apache-2.0) - Image processing and texture mapping utilities.
