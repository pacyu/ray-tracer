# 🎨 Toy RayTracer: A Playground of Light

[![Language](https://img.shields.io/badge/Language-C++-blue.svg)](https://en.cppreference.com/)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)]()
[![License](https://img.shields.io/badge/License-MIT-green.svg)]()

[English](#english) | [中文](#chinese) | [日本語](#japanese)

---

<a name="english"></a>

## 🇺🇸 English

### 📖 About

A physically-based RayTracer built from scratch as a hobbyist project. Originally inspired by the "Ray Tracing in One Weekend" series, it is now evolving into a flexible rendering tool with advanced features.

### 🌟 Key Features

* **PBR Workflow**: Supports Lambertian, Metal, and Dielectric (Glass) materials.
* **Mathematical Shapes**: Native support for implicit surfaces (e.g., the 3D Heart Equation).
* **Importance Sampling**: Optimized light path calculation for faster noise reduction.
* **Parallel Rendering**: Powered by **OpenMP** for high-performance CPU rendering.
* **Image Output**: Integrated with **OpenCV** for image handling and storage.

### 🚀 Roadmap

- [ ] **Scene Configuration DSL**: A custom language to define scenes/objects without recompilation.
- [ ] **Cross-Platform Support**: Migration to CMake for Linux and macOS support.
- [ ] **Advanced Geometry**: Mesh loading (.obj) and complex SDF shapes.

---

<a name="chinese"></a>

## 🇨🇳 中文 (Chinese)

### 📖 关于项目

这是一个基于物理的光线追踪渲染器（RayTracer），起步于学习图形学的兴趣。该项目最初源自经典的“一周光线追踪”系列教程，目前正逐渐进化为一个功能丰富的渲染小工具。

### 🌟 核心特性

* **基于物理渲染 (PBR)**：支持漫反射、金属、电介质（玻璃）等多种材质。
* **数学之美**：内置隐式曲面支持（如经典的心形方程）。
* **重要性采样 (IS)**：通过采样权重优化，有效减少画面噪点。
* **多线程加速**：集成 **OpenMP**，充分压榨 CPU 核心性能。
* **图像存储**：利用 **OpenCV** 处理并保存高质量图像。

### 🚀 未来蓝图

- [ ] **场景配置语言**：自定义 DSL，无需重新编译即可动态定义物体、光源和相机。
- [ ] **跨平台支持**：支持 Linux 和 macOS。
- [ ] **几何体扩展**：增加三角面片（Mesh）和更复杂的 SDF 物体。

---

<a name="japanese"></a>

## 🇯🇵 日本語 (Japanese)

### 📖 プロジェクトについて

これは物理ベースのレイトレーシング・レンダラーです。最初は学習用の小さなプロジェクトとして始まりましたが、現在は機能拡張を続ける趣味のレンダリングツールへと進化しています。

### 🌟 主な機能

* **物理ベースレンダリング (PBR)**: 拡散反射、金属、ガラス（誘電体）などの質感を再現。
* **数学的な形状**: 隠関数による複雑な曲面（ハート方程式など）をサポート。
* **重要性サンプリング**: ノイズを効率的に抑えるための最適化アルゴリズムを搭載。
* **並列化**: **OpenMP** によるマルチスレッド高速レンダリング。
* **画像処理**: **OpenCV** を使用したレンダリング結果の保存。

### 🚀 ロードマップ

- [ ] **シーン設定言語**: コードを再コンパイルせずにシーンや物体を定義できる独自言語の実装。
- [ ] **マルチプラットフォーム**: Linux および macOS への対応。
- [ ] **高度なジオメトリ**: 三角メッシュの読み込みと SDF 形状の追加。

---

## 🛠️ Build & Setup (Windows)

1. **Environment**: Visual Studio 2017+ / VS Code (with CMake).
2. **Dependencies**:
   * [OpenCV](https://opencv.org/)
   * OpenMP (Enabled via Compiler Switch)
3. **Run**:

   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

## 📄 Credits

Originally based on Ray Tracing in One Weekend.
