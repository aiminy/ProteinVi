#include "MyWindow.h"

void MyWindow::drawAxis(){
  glLineWidth(1);
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glColor3f(1, 0, 0);
  glBegin(GL_LINES);
  glVertex3f(0, 0, 0);
  glVertex3f(1, 0, 0);
  glEnd();
  
  glColor3f(0, 1, 0);
  glBegin(GL_LINES);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 1, 0);
  glEnd();
  
  glColor3f(0, 0, 1);
  glBegin(GL_LINES);
  glVertex3f(0, 0, 0);
  glVertex3f(0, 0, 1);
  glEnd();
  
  glEnable(GL_LIGHTING);
  glPopMatrix();
}

#define BUFFER_SIZE 64
void MyWindow::processPick(){
  static GLuint selectBuff[BUFFER_SIZE];
  GLint hits, viewport[4];
  glSelectBuffer(BUFFER_SIZE, selectBuff);
  glGetIntegerv(GL_VIEWPORT, viewport);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glRenderMode(GL_SELECT);
  glLoadIdentity();
  gluPickMatrix(Fl::event_x(), viewport[3] - Fl::event_y(), 1, 1, viewport);
  GLfloat aspect = 1.0 * viewport[2] / viewport[3];
  gluPerspective(45, aspect, .5, 425);
  camera();
  drawData();
  
  hits = glRenderMode(GL_RENDER);
  if (hits == 1 ){
    static double r=.4;
    static double g=1;
    static double b=.3;
    
    if (fl_color_chooser("Set Chaint Color", r,g,b)){
      PDB_Parser* pdb = (PDB_Parser*)data; 
      pdb->getChaint(selectBuff[3] - 1)->setColor(r, g, b);
      rebuild();
    }
  }
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

void MyWindow::draw(){
  if (!valid()) {
    valid(1);
    glLoadIdentity();
    glViewport(0, 0, w(), h());    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    static GLfloat white[] = {.5, .5, .5, 1 };
    static GLfloat gray[] = { .2, .2, .2, 1 };
    static GLfloat shininess[] = { 60 };
    static GLfloat position[] = { 50, -50, 5, 0};
    static GLfloat position2[] = { -50, -50, 0, 0};
    
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_POSITION, position2);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_AMBIENT, gray);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_SHININESS, shininess);

    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    glLightfv(GL_LIGHT1, GL_SHININESS, shininess);
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
    
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,
		  GL_SEPARATE_SPECULAR_COLOR);
  }
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (float)w()/h(), .5, 300);
  glMatrixMode(GL_MODELVIEW);
  
  glPushMatrix();
  glLoadIdentity();
  camera();
      
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
  if (texture_name == TEXTURE_NAME_NULL){
    glGenTextures(1, &texture_name);
    glBindTexture(GL_TEXTURE_2D, texture_name);
    int w, h, c;
    unsigned char* array = open_png("texture.png", w, h, c, 1);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLfloat coef1[] = {1, 0, 0, 0};
    GLfloat coef2[] = {0, 1, 0, 0};
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_EYE_PLANE, coef1);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_T, GL_EYE_PLANE, coef2);
    
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, array);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, texture_name);
    free(array);
  }
  
  if (display == DISPLAY_LIST_NULL){
    display = glGenLists(1);
    glNewList(display, GL_COMPILE_AND_EXECUTE);
    drawAxis();
    drawData();
    glEndList();
  } else {
    glBindTexture(GL_TEXTURE_2D, texture_name);
    
    glCallList(display);
  }
  
  glPopMatrix();
}

void MyWindow::save(char* fileName){
  unsigned char* array = (unsigned char*)malloc(sizeof(int) * h() * w());
  glPixelZoom(1, -1);
  glReadPixels(0, 0, w(), h(), GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, array);
  
#ifndef _APPLE
  for (int i = 0; i < h(); i++)
      for (int j = 0; j < w(); j++){
	unsigned char b = array[(i * w() + j) * 4 + 0];
	array[(i * w() + j) * 4 + 0] = array[(i * w() + j) * 4 + 3];
	array[(i * w() + j) * 4 + 3] = b;
	b = array[(i * w() + j) * 4 + 1];
	array[(i * w() + j) * 4 + 1] = array[(i * w() + j) * 4 + 2];
	array[(i * w() + j) * 4 + 2] = b;
      }
  
#endif  
  int len = strlen(fileName);
  if (!strcmp(fileName + len - 3, "png") || !strcmp(fileName + len - 3, "PNG"))
    write_png(fileName, array, w(), h(), 1);
  
  
  if (!strcmp(fileName + len - 3, "jpg") || !strcmp(fileName + len - 3, "JPG") ||
      !strcmp(fileName + len - 4, "jpeg") || !strcmp(fileName + len - 4, "JPEG")){
    unsigned char* array2 = (unsigned char*)malloc(3 * h() * w());
    for (int i = 0; i < h(); i++)
      for (int j = 0; j < w(); j++){

	array2[(i * w() + j) * 3 + 0] = array[(i * w() + j) * 4 + 0];
	array2[(i * w() + j) * 3 + 1] = array[(i * w() + j) * 4 + 1];
	array2[(i * w() + j) * 3 + 2] = array[(i * w() + j) * 4 + 2];

      }
    compress(array2, fileName, w(), h(), 3);
    delete[] array2;
  }
  delete[] array;  
}

void MyWindow::camera(){
  double ex = d * cos(theta) * sin(alpha);
  double ey = d * cos(theta) * cos(alpha);
  double ez = d * sin(theta);  
  gluLookAt(ex, ey, ez, 0, 0, 0, 0, 0, 1);
}

int MyWindow::handle(int event){
  PDB_Parser *pdb = NULL;
  if (type == DATA_TYPE_PDB){
    pdb = (PDB_Parser*)data;
  }
  switch(event){
  case FL_MOUSEWHEEL:
    d += 3 * Fl::event_dy();
    redraw();
    break;
  case FL_KEYBOARD:
    switch (Fl::event_key()){
    case 'a': d += 5; redraw(); break;
    case 'z': d -= 5; redraw(); break;
    case 'q': mode = (mode + 1) % 3; redraw(); break;
    case '1': if (pdb) pdb->toggleMode(DRAW_MODE_ATOMS); rebuild();break;
    case '2': if (pdb) pdb->toggleMode(DRAW_MODE_BONDS); rebuild(); break;
    case '3': if (pdb) pdb->toggleMode(DRAW_MODE_NO_SIDE_CHAINTS); rebuild(); break;
    case '4': if (pdb) pdb->toggleMode(DRAW_MODE_CURVES); rebuild(); break;
    case '5': if (pdb) pdb->toggleMode(DRAW_MODE_CHAINT_VECTOR); rebuild(); break;
    case '6': if (glIsEnabled(GL_TEXTURE_2D)) glDisable(GL_TEXTURE_2D); else glEnable(GL_TEXTURE_2D);
      rebuild(); 
      break;
    case '7':{
      unsigned char* array = (unsigned char*)malloc(sizeof(int) * h() * w());
      glPixelZoom(1, -1);
      glReadPixels(0, 0, w(), h(), GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, array);
      unsigned char* array2 = (unsigned char*)malloc(sizeof(int) * 256 * 256);
      gluScaleImage(GL_RGBA, w(), h(), GL_UNSIGNED_BYTE, array, 
		    256, 256, GL_UNSIGNED_BYTE, array2); 
      glBindTexture(GL_TEXTURE_2D, texture_name);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, array2);
      free(array);
      free(array2);
      break;
    }
    
    case 'p': if (pdb) pdb->toggleMode(COLOR_MODE_AA_POLAR); rebuild(); break;
    case 'o': if (pdb) pdb->toggleMode(COLOR_MODE_AA_CHARGE); rebuild(); break;
    case 'i': if (pdb) pdb->toggleMode(COLOR_MODE_ATOM_TEMP); rebuild(); break;
    case 'u': if (pdb) pdb->toggleMode(COLOR_MODE_ATOM_TYPE); rebuild(); break;
    case 'm': save("protein_vis.png"); break;
    default: break;
    }
    break;

  case FL_PUSH:
    ox = Fl::event_x();
    oy = Fl::event_y();
    if (Fl::event_button() == FL_RIGHT_MOUSE){
      processPick();
    }
    break;

  case FL_DRAG:
    switch (Fl::event_button()){
    
    case FL_LEFT_MOUSE:{
      alpha += .005 * (Fl::event_x() - ox);
      theta += .005 * (Fl::event_y() - oy);
      if (alpha < 0) alpha = 2 * M_PI;
      if (alpha > 2 * M_PI) alpha = 0;
      
      if (theta < -M_PI/2) theta= -M_PI/2;
      if (theta > M_PI/2)  theta = M_PI/2;
      
      ox = Fl::event_x();
      oy = Fl::event_y();
      redraw();
      break;
    }
    
    }
  }
  return 0;
}

void MyWindow::drawData(){  
  switch (type){
  case DATA_TYPE_PDB:{
    
    PDB_Parser* pdb = (PDB_Parser*)data; 
    pdb->draw();
    
    break;
  }
    
  }
}
