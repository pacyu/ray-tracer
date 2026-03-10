# 🎨 Toy RayTracer: A Playground of Light

[![Language](https://img.shields.io/badge/Language-C++-blue.svg)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)](...)
[![Build](https://img.shields.io/badge/Build-CMake-orange.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](...)

[English](#english) | [中文](#chinese) | [日本語](#japanese)

---

<a name="english"></a>

## 🇺🇸 English

### 📖 About

A physically-based RayTracer built from scratch as a hobbyist project. Originally inspired by the "Ray Tracing in One Weekend" series, it is now evolving into a flexible rendering tool with advanced features.

**The highlight of this version is the **Auroric (.aur)** scene description language, allowing users to define geometry, materials, and lighting without recompiling the core engine.**

### 🌟 Key Features

* **Auroric DSL (.aur)**: A custom-designed, human-readable configuration language with a strict type system (int/float differentiation).
* **PBR Workflow**: Supports Lambertian, Metal, and Dielectric (Glass) materials.
* **Mathematical Shapes**: Native support for implicit surfaces (e.g., the 3D Heart Equation).
* **Importance Sampling**: Optimized light path calculation for faster noise reduction.
* **Parallel Rendering**: Powered by **OpenMP** for high-performance CPU rendering.
* **Image Output**: Integrated with **OpenCV** for image handling and storage.

### 🚀 Roadmap

- [x] **Scene Configuration DSL**: A custom language to define scenes/objects without recompilation.
- [ ] **Cross-Platform Support**: Migration to CMake for Linux and macOS support.
- [ ] **Advanced Geometry**: Mesh loading (.obj) and complex SDF shapes.

---

<a name="chinese"></a>

## 🇨🇳 中文 (Chinese)

### 📖 关于项目

这是一个基于物理的光线追踪渲染器（RayTracer），起步于学习图形学的兴趣。该项目最初源自经典的“一周光线追踪”系列教程，目前正逐渐进化为一个功能丰富的渲染小工具。

**本次重大更新引入了全新的场景描述语言 —— **Auroric (.aur)**。用户可以通过编写 `.aur` 脚本动态配置物体、材质、相机和光源，彻底摆脱了修改场景必须重新编译代码的烦恼。**

### 🌟 核心特性

* **Auroric DSL (.aur)**：自研场景配置语言，支持嵌套对象定义与多类型属性解析。
  + **严格类型系统**：基础属性严格区分 `int` (如 128) 与 `float` (如 128.0)。
  + **复合类型支持**：`Vec3` 与 `quaternion` 自动兼容整型与浮点输入。
* **基于物理渲染 (PBR)**：支持漫反射、金属、电介质（玻璃）等多种材质。
* **数学之美**：内置隐式曲面支持（如经典的心形方程）。
* **重要性采样 (IS)**：通过采样权重优化，有效减少画面噪点。
* **多线程加速**：集成 **OpenMP**，充分压榨 CPU 核心性能。
* **图像存储**：利用 **OpenCV** 处理并保存高质量图像。

### 🚀 未来蓝图

- [x] **场景配置语言**：自定义 DSL，无需重新编译即可动态定义物体、光源和相机。
- [ ] **跨平台支持**：支持 Linux 和 macOS。
- [ ] **几何体扩展**：增加三角面片（Mesh）和更复杂的 SDF 物体。

---

<a name="japanese"></a>

## 🇯🇵 日本語 (Japanese)

### 📖 プロジェクトについて

これは物理ベースのレイトレーシング・レンダラーです。最初は学習用の小さなプロジェクトとして始まりましたが、現在は機能拡張を続ける趣味のレンダリングツールへと進化しています。

**今回のアップデートでは、独自のシーン定義言語 **Auroric (.aur)** を導入しました。これにより、エンジンを再コンパイルすることなく、テキストファイルのみで自由にシーンを構築することが可能になりました。**

### 🌟 主な機能

* **Auroric DSL (.aur)**: シーン構成を直感的に記述できる独自言語。
  * 厳格な型システム: 基礎的な属性では、整数型（例：128）と浮点数型（例：128.0）を厳格に区別して解析します。

  * 柔軟な複合型: `Vec3` や `quaternion` などのベクトル形式では、内部の数値に整数と浮点数が混在していても自動的に処理されます。
* **物理ベースレンダリング (PBR)**: 拡散反射、金属、ガラス（誘電体）などの質感を再現。
* **数学的な形状**: 隠関数による複雑な曲面（ハート方程式など）をサポート。
* **重要性サンプリング**: ノイズを効率的に抑えるための最適化アルゴリズムを搭載。
* **並列化**: **OpenMP** によるマルチスレッド高速レンダリング。
* **画像処理**: **OpenCV** を使用したレンダリング結果の保存。

### 🚀 ロードマップ

- [x] **シーン設定言語**: コードを再コンパイルせずにシーンや物体を定義できる独自言語の実装。
- [ ] **マルチプラットフォーム**: Linux および macOS への対応。
- [ ] **高度なジオメトリ**: 三角メッシュの読み込みと SDF 形状の追加。

---

## 🛠️ Build & Setup (Windows)

### 1. Prerequisites

- **Compiler**: Visual Studio 2017+ (or GCC/Clang with C++17 support).
- **CMake**: Version 3.10 or higher.
- **[OpenCV](https://opencv.org/)**: Ensure OpenCV is installed. The default search path is `C:/OpenCV/build`. (Please update `OpenCV_DIR` in `CMakeLists.txt` if your path is different).

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

Then, after a short while, you will get an image like this:

![taubin's heart](/examples/image.png)

### 4. Auroric Script Example

**Note: Strictly distinguish between integers and floating-point numbers.**

```aur
# Example .aur scene file

Camera {
  image_size: (600, 600) # Integers for dimensions
  samples_per_pixel: 128 # Integer
  max_depth: 8           # Integer
  background: (0.0, 0.0, 0.0)
  lookfrom: (278.0, 278.0, -800.0)
  lookat: (278.0, 278.0, 0.0)
  vup: (0.0, 1.0, 0.0)
  fov: 40.0              # Strict float
}

DiffuseLight {
  albedo: (4, 4, 4)
}

Lambertian {
  albedo: (0.65, 0.05, 0.05)
}

XZRect {
  quaternion: (113, 443, 127, 432)
  k: 554.0
  material: DiffuseLight
}

Light {
  object: XZRect
}

Heart {
  center: (278, 278, 278)
  radius: 150.0          # Strict float
  material: Lambertian
}
```

## 📄 Credits

Originally based on Ray Tracing in One Weekend. Auroric DSL parser implemented with a custom recursive descent approach.
