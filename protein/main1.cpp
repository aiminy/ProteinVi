#include "MyWindow.h"
#include "def.h"
#include "pdb_parser.h"
#include "FL/Fl_Window.H"
#include "MainWindow.h"
#include "FL/Fl_Tiled_Image.H"
#include "FL/Fl_PNG_Image.H"

int main(int argc, char** args){
   
  MainWindow *main = new MainWindow(10, 10, 512, 600, "Protein Vis");
  Fl_Group group(0, 0, 512, 600);
  group.image(
	      new Fl_Tiled_Image(
				 new Fl_PNG_Image("test.png")));
  group.align(FL_ALIGN_INSIDE);
  MyWindow* win = new MyWindow(0, 0, 512, 512, "OpenGL");
 
  main->setGL(win);
  main->setup();
  group.end();
  group.resizable(win);
  main->resizable(group);
  main->show();
  win->show();
  return Fl::run();
}
