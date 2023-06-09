#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader
{
public:
	// program id
	unsigned int ID;

	// reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	// use shader
	void employ();
	// utility uniform function
	void setBool(const std::string& name, bool value) const; // doesnt change member functions
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setVec3(const std::string& name, const glm::vec3& data) const;
};

