#ifndef __PDB_PARSE
#define __PDB_PARSE

#include "def.h"

#define PDB_LINE_LENGTH 80
#define PDB_MAX_CHAINTS 30


#define DRAW_MODE_ATOMS             1
#define DRAW_MODE_BONDS             2
#define DRAW_MODE_NO_SIDE_CHAINTS   4
#define DRAW_MODE_CURVES            8
#define DRAW_MODE_TEXT_GRID        16
#define DRAW_MODE_CHAINT_VECTOR    32


#define COLOR_MODE_CHAINT         256
#define COLOR_MODE_AA_POLAR       512
#define COLOR_MODE_AA_CHARGE     1024
#define COLOR_MODE_ATOM_TYPE     2048
#define COLOR_MODE_ATOM_TEMP     4096

using namespace std;

/**
 * parser for pdf 2.2 http://www.rcsb.org/pdb/docs/format/pdbguide2.2/guide2.2_frame.html
 */

class PDB_Parser{
 public:
  PDB_Parser(const char* fileName){
    _file = PDB_File(fileName);
    _numbChaints = 0;
    _mode = DRAW_MODE_BONDS | DRAW_MODE_NO_SIDE_CHAINTS;
    
    for (int i = 0; i < 64 * 4; i++){
      _strip[i] = (GLubyte)(i % 256);
    }
  }

  ~PDB_Parser(){
    for (int i = 0; i < _numbChaints; i++){
      delete(_chaints + i);
    }
  }
  int getNumbChaints() const {
    return _numbChaints;
  }
    
  Chaint* getChaint(int i){
    return _chaints + i;
  }
  int parse();
  
  void draw();
    
  void toggleMode(unsigned int m){
    _mode &=0xff;
    _mode = _mode ^ m;
    if (m == COLOR_MODE_AA_POLAR && (_mode & COLOR_MODE_AA_CHARGE))
      _mode ^= COLOR_MODE_AA_CHARGE;
    
    if (m == COLOR_MODE_AA_CHARGE && (_mode & COLOR_MODE_AA_POLAR))
      _mode ^= COLOR_MODE_AA_POLAR;

    if (m == COLOR_MODE_ATOM_TYPE && (_mode & COLOR_MODE_ATOM_TEMP))
      _mode ^= COLOR_MODE_ATOM_TEMP;

    if (m == COLOR_MODE_ATOM_TEMP && (_mode & COLOR_MODE_ATOM_TYPE))
      _mode ^= COLOR_MODE_ATOM_TYPE;
    
    GLuint name;
    if (m & DRAW_MODE_TEXT_GRID){
    
    }
  }
  
 protected:
  unsigned int _mode;
  PDB_File _file;
  Chaint _chaints[PDB_MAX_CHAINTS];
  int _numbChaints;
  GLubyte _strip[64 * 4];
};
#endif
