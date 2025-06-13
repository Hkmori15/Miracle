# Miracle Engine Architecture

## Core Design Principles

1. **Modular Design**: Each system is self-contained with clear interfaces
2. **C23 Standards**: Modern C with snake_case naming conventions
3. **Minimal Dependencies**: Only essential libraries for core functionality
4. **Cross-Platform Ready**: GLFW and OpenGL for portability
5. **Memory Safety**: Explicit allocation/deallocation with error checking

## System Architecture

### Core Systems (`src/core/`)

#### Window Management (`window.h/c`)
- GLFW integration for cross-platform windowing
- OpenGL context creation and management
- Viewport handling and resize callbacks
- Error handling and cleanup

#### Input System (`input.h/c`)
- Keyboard input polling
- Mouse position and delta tracking
- First-frame jump prevention
- Mouse capture for FPS-style controls

### Rendering Pipeline (`src/renderer/`)

#### Shader System (`shader.h/c`)
- GLSL shader compilation and linking
- Uniform variable management (mat4, vec3, int, float)
- Error reporting for compilation/linking failures
- Resource cleanup

#### Texture System (`texture.h/c`)
- STB_image integration for image loading
- OpenGL texture creation and management
- Mipmap generation
- Multiple format support (RGB, RGBA, etc.)

#### Camera System (`camera.h/c`)
- First-person camera implementation
- WASD movement controls
- Mouse look with pitch constraints
- View matrix generation using cglm

#### Model System (`model.h/c`)
- Assimp integration for 3D model loading
- Vertex buffer object (VBO) management
- Vertex array object (VAO) setup
- Index buffer object (EBO) handling
- Procedural geometry generation (cube)

#### High-Level Renderer (`renderer.h/c`)
- Frame management (begin/end)
- Model submission and rendering
- Matrix management (model, view, projection)
- OpenGL state management

### Physics System (`src/physics/`)
- Bullet Physics
- World creation and management interface
- Rigid body addition framework
- Transform extraction interface

### Audio System (`src/audio/`)
- OpenAL
- 3D positional audio framework
- Sound loading and playback interface
- Listener position management

## Data Flow

```
Main Loop
├── Input Update
│   ├── Keyboard polling
│   └── Mouse delta calculation
├── Engine Update
│   ├── Camera movement
│   ├── Physics simulation
│   └── Audio listener update
└── Rendering
    ├── Begin frame (clear buffers)
    ├── Set matrices (projection, view, model)
    ├── Bind shaders and textures
    ├── Submit models for rendering
    └── End frame (swap buffers)
```

## Memory Management

- **Explicit Allocation**: All dynamic memory explicitly managed
- **Error Checking**: All allocations checked for failure
- **Resource Cleanup**: Proper cleanup functions for all systems
- **RAII-style**: Resource acquisition tied to initialization

## OpenGL Integration

- **Modern OpenGL**: Uses OpenGL 4.1 Core Profile
- **Extension Loading**: Custom lightweight extension loading
- **State Management**: Minimal state changes for performance
- **Error Handling**: OpenGL error checking in debug builds

## Build System

- **CMake**: Modern CMake with pkg-config integration
- **Dependencies**: Automatic library detection and linking
- **Assets**: Automatic asset copying to build directory
- **Standards**: C23 standard with appropriate compiler flags

## Testing Framework

- **Manual Testing**: Interactive controls and visual verification
- **Asset Generation**: Procedural test texture creation

## Extension Points

The architecture is designed for easy extension:

1. **New Renderers**: Implement renderer interface
2. **Asset Pipeline**: Extend model and texture loaders
3. **Scene Management**: Add scene graph system
4. **Component System**: Add ECS architecture

## Performance Considerations

- **Minimal Allocations**: Reduce runtime memory allocation
- **Batch Rendering**: Group similar draw calls
- **State Caching**: Minimize OpenGL state changes
- **Frustum Culling**: Ready for view frustum culling
- **LOD System**: Framework for level-of-detail

## Future Enhancements

1. **Scene Graph**: Hierarchical scene management
2. **Asset Pipeline**: Binary asset format
3. **Lighting System**: Advanced lighting techniques
4. **Post-Processing**: Render target framework
5. **More primitive shapes**: Add more primitive shapes
6. **Asset management system**: Asset management system for resource
7. **UI System**: Immediate mode GUI integration
