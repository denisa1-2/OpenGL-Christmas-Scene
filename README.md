# OpenGL-Christmas-Scene
A 3D computer graphics project that renders an interactive Christmas winter village using **C++**, **OpenGL**, **GLFW**, **GLEW**, **GLM**, and **Blender**.

## Overview

This project was developed as a university computer graphics assignment.
The scene represents a **small winter village during Christmas**, built and arranged in **Blender**, then integrated into a real-time **OpenGL** application.

To create the final environment, I combined:
- **3D assets sourced online**
- **scene composition and arrangement done by me in Blender**
- **interactive rendering and visual effects implemented in OpenGL**

The application allows real-time exploration of the scene and includes multiple graphics features such as camera navigation, global scene transformations, skybox switching, fog, snowfall particles, animated elements, and shadow mapping.

## Project Highlights

- Built an interactive **3D winter village scene**
- Integrated **Blender-exported OBJ/MTL models** into an OpenGL project
- Implemneted **FPS-style camera movement** and mouse-based rotation
- Added **cinematic intro camera animation**
- Implemented **day/night switching** with different skyboxes and lighting setups
- Added **atmospheric effects** such as fog and snowfall
- Implemented **shadow mapping** using depth framebuffer and depth texture
- Added multiple **render modes**: solid, wireframe, points, smooth
- Added **scene-wide transformations**: translation, rotation, scaling
- Used **vertex shaders** and **fragment shaders** for the rendering pipeline

## Scene Description

This scene represents a Christmas-themed winter village centered around a paved area.
Its main elements include:
- snow-covered ground
- cabins and vegetation
- a central Christmas tree
- decorative objects such as a sled, gifts, and snowmen
- lanterns that illuminate the environment at night

The scene was modeled and assembled in Blender, then exported and renderd in OpenGL.

## Technologies Used

- **C++**
- **OpenGL 4.1 Core Profile**
- **GLFW**
- **GLM**
- **Blender**
- **GLSL shaders**

## Implemented Features
- FPS-style camera mocement
