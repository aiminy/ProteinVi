#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <FL/glut.H>
#include "pdb_parser.h"
#include "imageio.h"
#include "FL/Fl_Color_Chooser.H"

#ifdef _APPLE
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <math.h>

#include "def.h"

#define DATA_TYPE_NULL         0
#define DATA_TYPE_PDB          1

#define DISPLAY_LIST_NULL      9999
#define TEXTURE_NAME_NULL      9999

#ifndef __MY_WINDOW
#define __MY_WINDOW
class MyWindow : public Fl_Gl_Window{
 public:
  void draw();
  int handle(int);
  void save(char*);
  void setData(int type, void* data){
    this->type = type;
    this->data = data;   
    rebuild();
  }
  
  void rebuild(){
    if (display != DISPLAY_LIST_NULL){
      glDeleteLists(display, 1);
      display = DISPLAY_LIST_NULL;
    }
    redraw();
  }

  MyWindow(int x, int y, int w, int h, const char* t):
    Fl_Gl_Window(x, y, w, h, t){
    theta = M_PI / 2;
    alpha = M_PI / 2;
    d = 50;
    mode = 0;
    type = 0;
    data = 0;
    display = DISPLAY_LIST_NULL;
    texture_name = TEXTURE_NAME_NULL;
  }
 
 protected:

  void drawData();
  void camera();
  void drawAxis();
  void processPick();
  int mode;
  GLuint texture_name;
  GLuint display;
  void* data;
  int type;

  //camera variables
  double theta, alpha, d;
  int ox, oy;
  
};
#endif
