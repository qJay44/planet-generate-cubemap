#include "ComputeShader.hpp"

#include <format>

#include "utils/utils.hpp"
#include "utils/clrp.hpp"

static GLuint load(const fspath& filename, int type) {
  std::string shaderStr = readFile(filename);
  const char* shaderStrPtr = shaderStr.c_str();
  GLuint shaderId = glCreateShader(type);
  glShaderSource(shaderId, 1, &shaderStrPtr, NULL);

  return shaderId;
}

static GLuint compile(const fspath& filename, int type) {
  GLuint shaderId = load(filename, type);
  GLint hasCompiled;
  char infoLog[1'024];

  glCompileShader(shaderId);
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &hasCompiled);

  // if GL_FALSE
  if (!hasCompiled) {
    glGetShaderInfoLog(shaderId, 1'024, NULL, infoLog);
    std::string fmt = clrp::prepare(clrp::ATTRIBUTE::BOLD, clrp::FG::RED);
    std::string head = std::format("\n===== Shader compilation error ({}) =====\n\n", filename.string().c_str());
    printf(fmt.c_str(), head.c_str());
    puts(infoLog);
    for (size_t i = 0; i < head.length(); i++)
      printf(fmt.c_str(), "=");
    puts("");
    exit(1);
  }

  return shaderId;
}

static void link(GLuint program) {
  GLint hasLinked;
  char infoLog[1'024];

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &hasLinked);

  // if GL_FALSE
  if (!hasLinked) {
    glGetProgramInfoLog(program, 1'024, NULL, infoLog);
    std::string fmt = clrp::prepare(clrp::ATTRIBUTE::BOLD, clrp::FG::RED);
    printf(fmt.c_str(), "\n===== Shader link error =====\n\n");
    puts(infoLog);
    printf(fmt.c_str(), "=============================\n\n");
    exit(1);
  }
}

Shader::Shader() : program(0) {}

Shader::Shader(const fspath& vert, const fspath& frag, const fspath& geom) {
  program = glCreateProgram();
  GLuint shaders[3];
  u8 idx = 0;

  shaders[idx++] = compile(vert, GL_VERTEX_SHADER);
  shaders[idx++] = compile(frag, GL_FRAGMENT_SHADER);

  if (!geom.empty()) shaders[idx++] = compile(geom, GL_GEOMETRY_SHADER);

  for (int i = 0; i < idx; i++)
    glAttachShader(program, shaders[i]);

  link(program);

  for (int i = 0; i < idx; i++)
    glDeleteShader(shaders[i]);
}

Shader::Shader(const fspath& path) {
  program = glCreateProgram();
  GLuint shader = compile(path, GL_COMPUTE_SHADER);
  glAttachShader(program, shader);
  link(program);
  glDeleteShader(shader);
}

void Shader::use() const { glUseProgram(program); }

void Shader::setUniform1i(const std::string& name, const GLint& val) const {
  use();
  glUniform1i(glGetUniformLocation(program, name.c_str()), val);
}

void Shader::setUniform1ui(const std::string& name, const GLuint& val) const {
  use();
  glUniform1ui(glGetUniformLocation(program, name.c_str()), val);
}

void Shader::setUniform2ui(const std::string& name, const uvec2& v) const {
  use();
  glUniform2ui(glGetUniformLocation(program, name.c_str()), v.x, v.y);
}

void Shader::setUniform3f(const std::string& name, const vec3& v) const {
  use();
  glUniform3f(glGetUniformLocation(program, name.c_str()), v.x, v.y, v.z);
}

void Shader::setUniformTexture(const std::string& name, const GLuint& unit) const { setUniform1i(name, unit); }
