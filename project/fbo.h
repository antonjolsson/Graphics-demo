#pragma once

#include <GL/glew.h>
#include <vector>

class Fbo {
public:
	GLuint framebufferId;
	std::vector<GLuint> colorTextureTargets; 
	GLuint depthBuffer;
	int width;
	int height;
	bool isComplete;

	explicit Fbo(int _numberOfColorBuffers = 1);
		
	void resize(int _w, int _h);
	bool checkFramebufferComplete(void);
};
