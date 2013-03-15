#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <GL/glew.h>
#include <map>
#include <string>

class TextureManager{
public:
	~TextureManager(void);
	TextureManager(void);
	GLuint load(const std::string& filename);
	bool freeTexture(std::string);
	bool freeTexture(GLuint);
private:
	//Consider using Boost Multi-index for looking up either by string or by texture id
	std::map<std::string, GLuint> textures_;
};

#endif