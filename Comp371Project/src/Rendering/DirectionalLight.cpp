#include "DirectionalLight.h"

std::shared_ptr<OpenGLShader> DirectionalLight::s_shader;
Material DirectionalLight::s_glowMat = Material(true);
