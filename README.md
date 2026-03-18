# OpenGL-Christmas-Scene

## Overview
OpenGL Christmas Scene is a 3D computer graphics project that renders an interactive winter village environment using **C++ and OpenGL**.

The scene was created in **Blender**, using a combination of online 3D assets and manual scene composition. It was then integrated into an OpenGL application where I implemented real-time rendering, interaction, and multiple visual effects.

---

## Features
- Interactive 3D winter village scene
- Blender scene integration into OpenGL (OBJ + MTL + textures)
- Real-time camera movement and mouse control
- Intro cinematic camera animation
- Day / Night mode switching
- Skybox rendering (day and night environments)
- Fog effect
- Snow particle system
- Shadow mapping (depth map + lighting pass)
- Multiple render modes:
  - Solid
  - Wireframe
  - Points
  - Smooth
- Scene transformations:
  - Translation
  - Rotation
  - Scaling
- GLSL Vertex and Fragment Shader usage

---

## How It Works

The application renders a 3D scene in real time using the OpenGL pipeline:

1. The scene is modeled in Blender and exported as `.obj` with `.mtl` and textures  
2. Models are loaded into the application using a custom Model3D loader  
3. The camera system computes the view matrix for navigation  
4. Transformations (model, view, projection) are applied using GLM  
5. Lighting and shading are computed in GLSL shaders  
6. A depth map is generated for shadow mapping  
7. The final scene is rendered with lighting, shadows, fog, and optional effects  

---

## Technologies Used
- C++
- OpenGL 4.1 Core Profile
- GLFW
- GLEW
- GLM
- Blender
- GLSL (Vertex & Fragment Shaders)

---

## Controls

| Key | Action |
|---|---|
| `W A S D` | Move camera |
| `Mouse` | Rotate camera |
| `Q / E` | Rotate scene |
| `Arrow Keys` | Translate scene |
| `+ / -` | Scale scene |
| `R` | Reset transformations |
| `J / L` | Rotate light |
| `N` | Toggle Day/Night |
| `C` | Toggle Fog |
| `Z` | Toggle Snow |
| `O / F / P / M` | Render modes |
| `ESC` | Exit |

---

## Project Structure

```text
proiect/
|- main.cpp
|- Camera.*
|- Model3D.*
|- Shader.*
|- SkyBox.*
|- shaders/
|- textures/
|- skybox_zi/
|- skybox_noapte/
```

## Running the Project

### Requirements
- Visual Studio (recommended)
-OpenGL-compatible GPU
- Installed libraries:
   - GLFW
   - GLEW
   - GLM
 
Note:
Make sure that the required OpenGL libraries (GLFW, GLEW, GLM) are correctly installed and configured in your project.
Depending on your setup, this may involve placing the libraries in the project directory or configuring them through Visual Studio settings (include directories and linker settings)

### Steps
1. Clone the repository:
   ```bash
   git clone https://github.com/denisa1-2/OpenGL-Christmas-Scene.git
2. Open `proiect.sln` in Visual Studio
3. Make sure all dependencies are properly configured
4. Build and run the project

---

## Notes
If the project does not run, ensure that:
- the include paths for GLFW, GLEW, and GLM are correctly set
- the linker settings include the required `.lib` files
- all required `.dll` files are available in the executable directory

---

## Usage

1. Run the application
2. Explore the sene using keyboard and mouse
3. Toggle effects such as fog, snow, and day/night mode
4. Switch vetween different renderin modes
5. Observe lighting, shadows, and environmental effects in real time

---

## Why I Built This Project

This project was developed as a part of a computer graphics course.

The goal was to understand how real-time 3D rendering works using:
- camera systems
- transformations
- lighting models
- shader programming
- scene management

Additionally, I learned how to work in Blender and also explored how to integrate Blender-created scenes into a C++ graphics application.

---

## Possible Improvements
- Add post-processing effects (bloom, HDR)
- Improve shadow quality
- Add more animations and interactions
- Optimize rendering performance
- ADD UI controls

## Screenshots

### 1. Day Scene
<img width="1905" height="975" alt="image" src="https://github.com/user-attachments/assets/6724696c-c70f-4a1d-bea2-b6048275587f" />

### 2. Night Scene
<img width="1909" height="984" alt="image" src="https://github.com/user-attachments/assets/cc4d5bff-32b5-44fd-857e-cdb340f3a1f0" />

### 3. Snow and Fog
<img width="1910" height="981" alt="image" src="https://github.com/user-attachments/assets/c3713f16-6a69-4c6e-894f-82461e39c28d" />

### 4. Wireframe Mode
<img width="1906" height="979" alt="image" src="https://github.com/user-attachments/assets/f8a1a678-da41-422c-9cdb-08c31783585b" />

---

## Author
Răileanu Iulia-Denisa 
