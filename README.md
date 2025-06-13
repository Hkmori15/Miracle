# Miracle

A minimal 3D game engine written in C. 🌸

## Features

- **Core Systems**
  - GLFW-based window management
  - Input handling with mouse delta tracking
  - First-person camera with WASD + mouse look

- **Renderer**
  - OpenGL 4.1 Core Profile
  - Shader compilation and management
  - Texture loading with stb_image
  - 3D model loading with Assimp
  - Basic Phong lighting

- **Physics**
  - Bullet Physics integration framework
  - Rigid body dynamics

- **Audio**
  - OpenAL framework
  - 3D positional audio support

## Technology Stack

- **Language**: C
- **Windowing**: [GLFW](https://github.com/glfw/glfw)
- **OpenGL Loading**: GLEXT
- **Math Library**: [cglm](https://github.com/recp/cglm)
- **Image Loading**: [stb_image](https://github.com/nothings/stb)
- **Model Loading**: [Assimp](https://github.com/assimp/assimp)
- **Physics**: [Bullet Physics](https://github.com/bulletphysics/bullet3)
- **Audio**: [OpenAL](https://github.com/kcat/openal-soft)

## Building

### Prerequisites

Install the required dependencies:

```bash
# Ubuntu/Debian
sudo apt install libglfw3-dev libcglm-dev libassimp-dev

# Arch Linux/Artix Linux
sudo pacman -S glfw assimp
yay -S cglm

# macOS (with Homebrew)
brew install glfw cglm assimp
```

### Compilation

```bash
# CMake
mkdir build
cd build
cmake ..
make

# Sh build
./build.sh
```

### Running

```bash
./miracle
```

## Quick Start

1. **Clone and build:**
   ```bash
   git clone https://github.com/Hkmori15/Miracle.git
   cd miracle
   ./build.sh
   ```

2. **Create test texture:**
   ```bash
   gcc create_texture.c -o create_texture && ./create_texture
   ```

3. **Run the engine:**
   ```bash
   cd build
   ./miracle
   ```

## Project Structure

```
miracle/
├── src/
│   ├── core/           # Core engine systems
│   │   ├── window.h/c  # Window management
│   │   └── input.h/c   # Input handling
│   ├── renderer/       # Rendering systems
│   │   ├── shader.h/c  # Shader management
│   │   ├── texture.h/c # Texture loading
│   │   ├── camera.h/c  # Camera system
│   │   ├── model.h/c   # 3D model loading
│   │   └── renderer.h/c # High-level renderer
│   ├── physics/        # Physics systems
│   │   └── physics.h/c # Bullet Physics wrapper
│   ├── audio/          # Audio systems
│   │   └── audio.h/c   # OpenAL wrapper
│   └── main.c          # Main application
├── assets/
│   └── shaders/        # GLSL shaders
│       ├── basic.vert  # Vertex shader
│       └── basic.frag  # Fragment shader
├── include/
│   └── stb/            # STB libraries
└── CMakeLists.txt      # Build configuration
```

## Coding Standards

- **Naming**: snake_case for functions and variables
- **Module Prefixes**: All public functions prefixed with module name (e.g., `renderer_init()`)
- **Memory Management**: Explicit allocation/deallocation with error checking

## Current Status

This is a foundational implementation with:
- ✅ **Working window and input systems** - GLFW integration with mouse capture
- ✅ **Basic 3D rendering with shaders** - OpenGL 4.1+ with vertex/fragment shaders
- ✅ **Camera system with first-person controls** - WASD movement + mouse look
- ✅ **Model loading framework** - Assimp integration for 3D models
- ✅ **Texture loading** - STB_image integration with BMP/PNG/JPG support
- ✅ **Procedural cube generation** - Built-in test geometry
- ✅ **Basic Phong lighting** - Ambient, diffuse, and specular lighting
- ✅ **Modular architecture** - Clean separation of concerns
- ✅ **Physics integration** - Ready physics with Bullet
- ✅ **Audio system** - Ready audio with OpenAL support MP3/OGG/WAV

## License

MIT
