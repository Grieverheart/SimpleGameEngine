#include "include/texture.h"
#include "include/bitmap.h"
#include <FreeImage.h>
#include <iostream>

#define TEX_ROOT_DIR "resources/Textures/"

TextureManager::~TextureManager(void){
	for(auto texture: textures_){
		glDeleteTextures(1, &(texture.second));
	}
	FreeImage_DeInitialise();
}

TextureManager::TextureManager(void){
	FreeImage_Initialise();
}

bool TextureManager::freeTexture(std::string texName){
	auto texture = textures_.find(texName);
	if(texture != textures_.end()){
		glDeleteTextures(1, &(texture->second));
		textures_.erase(texture);
		return true;
	}
	return false;
}

bool TextureManager::freeTexture(GLuint texID){
	for(auto texture = textures_.begin(); texture != textures_.end(); texture++){
		if(texture->second == texID){
			glDeleteTextures(1, &(texture->second));
			textures_.erase(texture);
			return true;
		}
	}
	return false;
}

GLuint TextureManager::load(const std::string& filename){
    GLenum textureTarget = GL_TEXTURE_2D;
	
	auto texture = textures_.find(filename);
	if(texture != textures_.end()) return texture->second;
	
	FREE_IMAGE_FORMAT img_type = FreeImage_GetFileType((TEX_ROOT_DIR+filename).c_str(), 0);
	if(img_type == FIF_UNKNOWN) img_type = FreeImage_GetFIFFromFilename((TEX_ROOT_DIR+filename).c_str());
	FIBITMAP* image = FreeImage_Load(img_type, (TEX_ROOT_DIR+filename).c_str(), 0);
	
	if(image){
	
		unsigned int img_width  = FreeImage_GetWidth(image);
		unsigned int img_height = FreeImage_GetHeight(image);
		GLenum format 			= FreeImage_IsTransparent(image)? GL_BGRA : GL_BGR;
		GLenum internal_format  = FreeImage_IsTransparent(image)? GL_RGBA8: GL_RGB8;
		
		GLuint textureObj;
		glGenTextures(1, &textureObj);
		glBindTexture(textureTarget, textureObj);
		glTexImage2D(textureTarget, 0, internal_format, img_width, img_height, 0, format, GL_UNSIGNED_BYTE, FreeImage_GetBits(image));
		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(textureTarget);
		FreeImage_Unload(image);
		textures_[filename] = textureObj;
		return textureObj;
	}
	else return 0;
}
