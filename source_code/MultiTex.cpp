#include "MultiTex.h"


#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = NULL;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;
#endif

void initMultiTex()
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__))
  glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
  glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
  glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
#endif
}

void activateTexture(unsigned int textureUnit, unsigned int textureType)
{
  glActiveTextureARB(textureUnit);
  glEnable(textureType);
}
