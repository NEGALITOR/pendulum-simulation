#ifndef GLOBJECTS_H
#define GLOBJECTS_H

#include "opengl.h"
#include <utility>


// Default constructs to id = 0 (no glGen), call generate() after GL context is ready

// RAII wrapper for a VAO
struct GLVAO {
    GLuint id = 0;
    GLVAO() = default;
    ~GLVAO() { if (id) glDeleteVertexArrays(1, &id); }
    GLVAO(const GLVAO&)            = delete;
    GLVAO& operator=(const GLVAO&) = delete;
    GLVAO(GLVAO&& o) noexcept : id(std::exchange(o.id, 0)) {}
    GLVAO& operator=(GLVAO&& o) noexcept {
        if (this != &o) { if (id) glDeleteVertexArrays(1, &id); id = std::exchange(o.id, 0); }
        return *this;
    }
    void generate() { glGenVertexArrays(1, &id); }
    operator GLuint() const { return id; }
};

// RAII wrapper for an OpenGL buffer object (VBO and EBO)
struct GLBuffer {
    GLuint id = 0;
    GLBuffer() = default;
    ~GLBuffer() { if (id) glDeleteBuffers(1, &id); }
    GLBuffer(const GLBuffer&)            = delete;
    GLBuffer& operator=(const GLBuffer&) = delete;
    GLBuffer(GLBuffer&& o) noexcept : id(std::exchange(o.id, 0)) {}
    GLBuffer& operator=(GLBuffer&& o) noexcept {
        if (this != &o) { if (id) glDeleteBuffers(1, &id); id = std::exchange(o.id, 0); }
        return *this;
    }
    void generate() { glGenBuffers(1, &id); }
    operator GLuint() const { return id; }
};

#endif
