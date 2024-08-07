#pragma once

#include <GL/glew.h>
#include <cassert>

#define GLCall(x) x;assert(glGetError() == GL_NO_ERROR)
