#ifndef MULTITEX_H
#define MULTITEX_H

//#include <GL/gl.h>
#include "glext.h"

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))
#include <windows.h>

extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
#endif

void initMultiTex();
void activateTexture(unsigned int textureUnit, unsigned int textureType);

#endif //MULTITEX_H
