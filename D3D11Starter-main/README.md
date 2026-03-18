# Direct3D 11 Graphics Engine

This project was developed as part of **IGME-540: Foundations of Game Graphics Programming** at Rochester Institute of Technology.

The goal of the course was to learn how real-time rendering works at a low level by building a graphics engine using a hardware-accelerated graphics API.  
For this project, I implemented rendering features in C++ using Direct3D 11, including mesh rendering, lighting, shadow mapping, and post-processing.

Starter framework code was provided for window creation and basic Direct3D setup, and the rendering systems were implemented throughout the semester.

---

## Features

- Real-time rendering using Direct3D 11
- Custom rendering pipeline
- Mesh loading from OBJ files
- Material system with textures and shaders
- Multiple lights
- Shadow mapping
- Skybox rendering
- Post-processing effect (Chromatic Aberration)
- Camera movement and input handling
- Scene entities with transform system
- ImGui debug interface

---

## Technical Details

Language: C++  
Graphics API: Direct3D 11  
Shader Language: HLSL  
Math Library: DirectXMath  
Debug UI: ImGui  

Implemented systems:

- Graphics device and swap chain setup
- Render target and depth buffer management
- Vertex / index buffers
- Shader compilation and binding
- World / View / Projection matrices
- Shadow map rendering pass
- Post-processing pass
- Transform and camera system
- Basic scene entity structure

---

## Controls

- WASD — Move camera
- Mouse drag — Rotate camera
- Space / X — Move up / down
- ImGui panel — Adjust rendering settings

---

## Course Information

IGME-540 — Foundations of Game Graphics Programming  
Rochester Institute of Technology

Students built a real-time graphics engine using a low-level graphics API and learned about scene graphs, rendering pipelines, shaders, and real-time performance.

---

## Author

Hyunwoo Park  
Game Design & Development  
Rochester Institute of Technology