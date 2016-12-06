
#ifndef __MAIN_WINDOW
#define __MAIN_WINDOW
#include "FL/Fl_Window.H"
#include "MyWindow.h"
#include "FL/Fl_Button.H"
#include "FL/Fl_File_Chooser.H"
#include "FL/Fl_Color_Chooser.H"
#include "FL/Fl_Menu_Bar.H"
#include "FL/Fl_Toggle_Button.H"
#include "FL/Fl_Choice.H"
#include "pdb_parser.h"

#include <FL/Fl_Tiled_Image.H>



PDB_Parser *pdb;
MyWindow *glWin;
void cb_open(Fl_Widget* w, void* data){
  if (!pdb) free(pdb);
  Fl_File_Chooser fc(".", "*.pdb", Fl_File_Chooser::SINGLE, "Open PDB File");
  fc.show();
  while (fc.visible())
    Fl::wait();
  pdb = new PDB_Parser(fc.value());
  pdb->parse();
  glWin->setData(DATA_TYPE_PDB, pdb);
  glWin->draw();
}

void cb_save(Fl_Widget* w, void* data){
  if (!glWin) return;
  Fl_File_Chooser fc(".", "PNG Files (*.png)\tJPEG Files (*.{jpeg, jpg})", Fl_File_Chooser::CREATE, "Save Screenshot");
  fc.show();
  while (fc.visible())
    Fl::wait();
  if (fc.value())
    glWin->save((char*)fc.value());
  
}

void cb_menu(Fl_Widget* w, void* data){
  Fl_Menu_* mw = (Fl_Menu_*)w;
  const Fl_Menu_Item* m = mw->mvalue();
  unsigned int flag = (unsigned int)m->user_data_;
  
  if (!strcmp("&Chaints", m->label())) flag = COLOR_MODE_CHAINT;
  if (!strcmp("&AA Polarity", m->label())) flag = COLOR_MODE_AA_POLAR;
  if (!strcmp("&AA Charge", m->label())) flag = COLOR_MODE_AA_CHARGE;
  if (!strcmp("&Atom Temp", m->label())) flag = COLOR_MODE_ATOM_TEMP;
  if (!strcmp("&Atom Type", m->label())) flag = COLOR_MODE_ATOM_TYPE;
  //printf("%s %d\n", m->label(), flag);
  if (pdb)
    pdb->toggleMode(flag);
  glWin->rebuild();
}

Fl_Menu_Item menu_file[] ={
  {"&Open PDB", FL_CTRL+'o', cb_open, 0, FL_MENU_DIVIDER},
  {"&Save Image", FL_CTRL+'s', cb_save, 0, FL_MENU_DIVIDER},
  {0}
};

Fl_Menu_Item menu_draw[] ={
  {"&Atoms", 0, 0, (void*)DRAW_MODE_ATOMS, FL_MENU_TOGGLE},
  {"&Bonds", 0, 0, (void*)DRAW_MODE_BONDS, FL_MENU_TOGGLE | FL_MENU_VALUE},
  {"&Side Chaints", 0, 0, (void*)DRAW_MODE_NO_SIDE_CHAINTS, FL_MENU_TOGGLE},
  {"&Curves", 0, 0, (void*)DRAW_MODE_CURVES, FL_MENU_TOGGLE},
  {"&Chaint Vector", 0, 0, (void*)DRAW_MODE_CHAINT_VECTOR, FL_MENU_TOGGLE},
  {0},
};

Fl_Menu_Item menu_color[] ={
  {"&Chaints", 0},
  {"&AA Polarity", 0},
  {"&AA Charge", 0},
  {"&Atom Temp", 0},
  {"&Atom Type", 0},
  {0}
};

class MainWindow : public Fl_Window{
 public:
  MainWindow(int x, int y, int h, int w, const char* title) : Fl_Window(x, y, h, w, title){
    glWin = NULL;
    pdb = NULL;
  } 
  
  void setup(){
    Fl_Menu_Button *menu_button1 = new Fl_Menu_Button(0, 522, 100, 30, "File");
    menu_button1->menu(menu_file);
    menu_button1->callback(cb_menu);

    Fl_Menu_Button *menu_button2 = new Fl_Menu_Button(110, 522, 100, 30, "Draw");
    menu_button2->menu(menu_draw);
    menu_button2->callback(cb_menu);

    Fl_Choice *menu_button3 = new Fl_Choice(220, 522, 100, 30);
    menu_button3->menu(menu_color);
    menu_button3->callback(cb_menu);
    
    
    
  }

  void setGL(MyWindow* win){
    glWin = win;
  }
 protected:
  int handle(int event){
    if (glWin){
      int w = glWin->w();
      int h = glWin->h();
      if (Fl::event_x() < w && Fl::event_y() < h)
	return glWin->handle(event); 
      else 
	return Fl_Window::handle(event);
    }
  }
};

#endif
