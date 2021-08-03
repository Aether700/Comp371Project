#include "PointLight.h"

std::shared_ptr<OpenGLShader> PointLight::s_shader;
Material PointLight::s_glowMat = Material(true);