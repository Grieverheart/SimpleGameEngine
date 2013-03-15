#ifndef __SHFBO_H
#define __SHFBO_H

#include <GL/glew.h>

class CShadowMapFBO{
public:
	CShadowMapFBO(void);
	~CShadowMapFBO(void);
	bool Init(unsigned int WindowWidth, unsigned int WindowHeght);
	void BindForWriting(void)const;
	void BindForReading(GLenum TextureUnit)const;
	void Resize(unsigned int WindowWidth, unsigned int WindowHeight)const;
private:
	GLuint m_fbo;
	GLuint m_shadowMap;
};

#endif