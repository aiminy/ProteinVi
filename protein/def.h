#ifndef __DEF
#define __DEF

#include "info.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include "tmatrix.h"
#include "MyWindow.h"
#define PDB_LINE_LENGTH 80


using namespace std;

typedef struct {
  float x;
  float y;
  float z;
  float temp;
  char name[8];
  int serial;
} pdb_atom;

/**
 * file reader
 */
class PDB_File{
   
 protected:
  FILE* _file;
  char _buffer[PDB_LINE_LENGTH + 1];
  int _lineNum;
  
 public:
  PDB_File(){
    _file = NULL;
  }

  PDB_File(const char* fileName){
    open(fileName);
  }
  
  void open(const char* fileName){
     _file = fopen(fileName, "r");
     _lineNum = 0;
  }

  void close(){
    if (_file) fclose(_file);
    _lineNum = 0;
    _file = NULL;
  }
  
  bool isOpen() const{
    return (_file != NULL);
  }
  
  const char* getLine() const{
    return _buffer;
  }
  
  const int getLineNumb() const{
    return _lineNum;
  }
  
  int next(){
    if (!isOpen()) return 0;
    memset(_buffer, 0, PDB_LINE_LENGTH + 1);
    int len = fread(_buffer, 1, PDB_LINE_LENGTH, _file);
    char nl;
    //skip new line char
    fread(&nl, 1, 1, _file);
    if (nl == '\r') fread(&nl, 1, 1, _file); //for windows "\r\n"
    _lineNum ++;
    if (len < PDB_LINE_LENGTH) close();
    return len;
  }
};

/**
 * get the type according to name 
 * returns -1 if unknown aa name 
 */
inline int getAAType(const char* a){
  for (int i = 0; i < 20; i++){
    if (!strncmp(a, aa_names[i], 3))
      return i;
  }
  return -1;
}

class AminoAcid{
 public:
  AminoAcid(){
    _type = UNKOWN;
    _standard_atoms = NULL;
  }

  /**
   * default draw function
   */
  void draw(int mode);

  /**
   * set color info
   */
  void setColor(int mode, pdb_atom* atom=NULL);
  
  /**
   * construct an aa according to aa name
   */
  AminoAcid(const char* name){
    int type = getAAType(name);
    if (type != -1) {
      _type = type;
      _standard_atoms = (pdb_atom**)
	calloc(aa_numb_atoms[type], sizeof(pdb_atom*)); 
    } else {
      _type = UNKOWN;
      _standard_atoms = NULL;
    }
 }

  AminoAcid(const int type){
    _type = type;
    _standard_atoms = (pdb_atom**)
      calloc(aa_numb_atoms[type], sizeof(pdb_atom*)); 
  }
    
  ~AminoAcid(){
    if (_standard_atoms) free(_standard_atoms);
  }
  
  AminoAcid& operator=(const AminoAcid& aa){
    memcpy(this, &aa, sizeof(AminoAcid));  
    return *this;
  }

  /**
   * returns the stanard number of atoms in this aa
   */
  inline int getNumbAtoms(){
    return aa_numb_atoms[_type];
  }
  
  /**
   * add atoms to the aa
   */
  void add(pdb_atom* atom){
    if (!atom) return;
    for (int i = 0; i < getNumbAtoms(); i++){
      if (!strcmp(aa_atoms[_type][i], atom->name)){
	_standard_atoms[i] = atom;
	return;
      }
    }
  }

  pdb_atom* get(int i){
    return _standard_atoms[i];
  }

  int getType(){ return _type;}
  pdb_atom** getAtoms(){ return _standard_atoms; }
  pdb_atom* getAtom(int i){ return _standard_atoms[i];}
  void print(){
    printf("%s\n", aa_names[_type]);
    for (int i = 0; i < getNumbAtoms(); i++){
      if (!_standard_atoms[i]){
	printf("\t%s: >>>> missing <<<<\n", aa_atoms[_type][i]);
	
      } else 
	printf("\t%s: %f\t%f\t%f\n", 
	       _standard_atoms[i]->name,
	       _standard_atoms[i]->x,
	       _standard_atoms[i]->y,
	       _standard_atoms[i]->z);
    }
    
  }
    
  void calculateCentroid(){
    _centroid[0] = _centroid[1] = _centroid[2] = 0;
    for (int i = 0; i < aa_numb_atoms[_type]; i++){
      pdb_atom* atom = get(i);
      if (!atom) continue;
      _centroid[0] = atom->x;
      _centroid[1] = atom->y;
      _centroid[2] = atom->z;
    }  
  }
 protected:
  int _type;
  pdb_atom** _standard_atoms;
  GLfloat _centroid[3]; //side chaint centroid
};

/*
 * contiguous aa's
*/
class Chaint{
 public:
  Chaint(){
    _centroid[0] = _centroid[1] = _centroid[2] = 0;
    _color[0] = 1.0 * rand() / RAND_MAX;
    _color[1] = 1.0 * rand() / RAND_MAX;
    _color[2] = 1.0 * rand() / RAND_MAX;
  }

  void setColor(double r, double g, double b){
    _color[0] = (GLfloat)r;
    _color[1] = (GLfloat)g;
    _color[2] = (GLfloat)b;
  }
  void add(int pos, AminoAcid* aa){
    _aa[pos] = *aa;
  }

  void setName(char name){
    _name = name;
  }

  AminoAcid& get(int i){
    return _aa[i];
  }

  int size(){
    return _aa.size();
  }
  void print(){
    printf(">>>>>>>> %c <<<<<<<<<\n", _name);
    for (unsigned int i = 0; i < _aa.size(); i++){
      _aa[i].print();
    }
  }

  GLfloat* getCentroid();
  void draw(int mode);
 protected:
  char _name;
  std::map<int, AminoAcid> _aa;
  GLfloat _centroid[3];
  GLfloat _color[3];
};
#endif
