#pragma once

#include "utils/types.hpp"

class Shader {
public:
  Shader();
  Shader(const fspath& vert, const fspath& frag, const fspath& geom = "");
  Shader(const fspath& comp);

  void use() const;

  void setUniform1i(const std::string& name, const GLint& val) const;
  void setUniform1ui(const std::string& name, const GLuint& val) const;
  void setUniform2ui(const std::string& name, const uvec2& v) const;
  void setUniform3f(const std::string& name, const vec3& v) const;
  void setUniformTexture(const std::string& name, const GLuint& unit) const;

private:
  GLuint program;
};
