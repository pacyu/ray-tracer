# 🎨 Toy RayTracer: A Playground of Light

[![Language](https://img.shields.io/badge/Language-C++-blue.svg)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-blue)](...)
[![C++ CI Build](https://github.com/pacyu/ray-tracer/actions/workflows/cmake-build.yml/badge.svg)](https://github.com/pacyu/ray-tracer/actions/workflows/cmake-build.yml)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](...)

[中文](#chinese) | [English](#english) | [日本語](#japanese)

---

![taubin's heart](/examples/image.png)

![Dynamics Ocean](/examples/output.mp4)

## 🚀 Latest Updates (2026.04.02)

- **项目重构**: 对各个部件进行了分类，现在的项目目录结构更便于管理材质、纹理、几何对象。
- **新增材质**: 增加 `Plastic`、`OceanMaterial`（这个名字其实我想改为Water，还在考虑中）、`GlossyPlastic`、`Cloth` 材质。
- **新增海洋可渲染对象**: `Ocean` 作为一个几何物体，通过 Phillips Spectrum([J Tessendorf · 2004](https://jtessen.people.clemson.edu/reports/papers_files/coursenotes2004.pdf)) 可以通过给定分辨率、风速、风向等创建。

## 🚀 Updates (2026.03.25)

- **Auroric 语法重构**: 完全重构了解释器后端，现在 `aur` 脚本写起来会更灵活，自然。代码示例参考[这里](/bin/scene.aur)。
- **性能优化**: 增加了 `BVH` 树进行场景管理以及命中判断加速，对于简单场景性能提升比较少，也做了一部分智能指针优化，但性能提升不明显。
- **新增纹理**: 增加 `ImageTexture`（图片纹理），支持图片作为纹理的渲染效果。

## 🚀 Updates (2026.03.16)

- **Auroric 语法重构**：全面支持变量引用、`=` 赋值表达式及材质/物体定义。
- **物理渲染增强**：引入 **Beer's Law**（比尔定律），支持高质量彩色玻璃渲染。
- **隐式曲面修复**：彻底解决 `Heart` 方程的表面裂纹问题，并修复了轴向旋转 (RotateX/Y/Z) 逻辑。
- **新增对象**：增加 `ConstantMedium`（参与介质），支持烟雾、体积雾效果。

## 🚀 Updates (2026.03.10)

本次重大更新引入了全新的场景描述语言 —— **`Auroric` (.aur)**。用户可以通过编写 `.aur` 脚本动态配置物体、材质、相机和光源，彻底摆脱了修改场景必须重新编译代码的烦恼。

---

<a name="chinese"></a>

## 🇨🇳 中文 (Chinese)

### 📖 关于项目

这是一个基于物理的光线追踪渲染器（RayTracer），起步于学习图形学的兴趣。该项目最初源自经典的“一周光线追踪”系列教程，后面会考虑增加物理引擎。目的是通过该项目学习计算机图形学、数学（辛几何、PDE、数值算法等）、语言（英语和日语）、物理学（广义相对论/天体/宇宙物理/量子物理等相关知识），以此来了解世界底层原理以及拓展知识，便于接触前沿学术。目前是利用CPU多线程加速渲染，只能做到离线渲染，后面会迁移到GPU，利用CUDA性能加速。

### 🌟 核心特性

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

### 🚀 未来蓝图

* [x] **场景配置语言**：自定义 DSL，无需重新编译即可动态定义物体、光源和相机。
* [x] **跨平台支持**：支持 Linux 和 Windows。
* [x] **Auroric 扩展**: `Random` 随机数生成。
* [ ] **Auroric 进阶**：为 Auroric 增加 `for` 循环支持。
* [x] **几何体扩展**：增加三角面片（Mesh）和更复杂的 SDF 物体。
* [ ] **第三方模型导入**: 增加支持 .obj 等格式的模型导入渲染功能。
* [ ] **非均匀密度材质**
* [ ] **黎曼几何/光线弯曲**
* [x] **海洋模拟**
* [ ] **流体模拟**
* [ ] **三体问题模拟**
* [ ] **卡通渲染**

---

<a name="english"></a>

## 🇺🇸 English

### 📖 About

A physically-based RayTracer built from scratch as a hobbyist project. Originally inspired by the "Ray Tracing in One Weekend" series, it is now evolving into a flexible rendering tool with advanced features.

### 🌟 Key Features

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

### 🚀 Roadmap

* [x] **Scene Configuration DSL**: A custom language to define scenes/objects without recompilation.
* [x] **Cross-Platform Support**: Migration to CMake for Linux and Windows support.
* [ ] **Advanced Geometry**: Mesh loading (.obj) and complex SDF shapes.

---

<a name="japanese"></a>

## 🇯🇵 日本語 (Japanese)

### 📖 プロジェクトについて

これは物理ベースのレイトレーシング・レンダラーです。最初は学習用の小さなプロジェクトとして始まりましたが、現在は機能拡張を続ける趣味のレンダリングツールへと進化しています。

### 🌟 主な機能

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

### 🚀 ロードマップ

* [x] **シーン設定言語**: コードを再コンパイルせずにシーンや物体を定義できる独自言語の実装。
* [x] **マルチプラットフォーム**: Linux および Windows への対応。
* [ ] **高度なジオメトリ**: 三角メッシュの読み込みと SDF 形状の追加。

---

## 🛠️ Build & Setup (Windows)

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

## 📄 Credits

Originally based on Ray Tracing in One Weekend. Auroric DSL parser implemented with a custom recursive descent approach.
