#include "pdb_parser.h"
#include <stdlib.h>

pdb_atom* parseAtom(const char* line, char* resName, int& resSeq, char& chaintID);



int PDB_Parser::parse(){
  while (_file.next()){
    const char* buffer = _file.getLine();
    /*atom*/
    if (!strncmp(buffer, "ATOM", 4)){
      char resName[3];
      int resSeq;
      pdb_atom* atom = NULL;
      AminoAcid* aa = NULL;   
      int seq = 0;
      char chaintID;
      while (strncmp(buffer, "TER", 3)){
	atom = parseAtom(buffer, resName, resSeq, chaintID);
	if (!atom) continue;
	if (seq != resSeq){
	  if (aa) {
	    //aa->print();
	    _chaints[_numbChaints].setName(chaintID);
	    _chaints[_numbChaints].add(seq - 1, aa);
	  }
	  seq = resSeq;
	  aa = new AminoAcid(resName);
	}
	if (aa) aa->add(atom);
	_file.next();
	buffer = _file.getLine();
      }
      //aa->print();
      _chaints[_numbChaints].add(seq - 1, aa);
      _numbChaints++;
    }
    
  }
  
  return 0;
}

pdb_atom* parseAtom(const char* line, char* resName, int& resSeq, char& chaintID){  
  
  pdb_atom* atom = (pdb_atom*)malloc(sizeof(pdb_atom));
  
  //atom_serial number  
  atom->serial = (unsigned)atoi(line + 6); 
  
  //atom name
  int j = 0;
  for (int i = 12; i <= 15; i++){
    if (line[i] != ' ' && line[i] != '\t'){
      atom->name[j++] = line[i];
    } 
  }

  atom->name[j] = 0;
  
  //alternative locations are discarded
  if (!j || (line[16] != ' ' && line[16] != 'A')) {
    free(atom);
    return NULL;
  }
  
  //x, y, z, temp
  //x, y, z, temp and occupancy
  char num[9];
  num[8] = 0;
  memcpy(num, line + 30, 8);
  atom->x = (float)atof(num);
  
  memcpy(num, line + 38, 8);
  atom->y = (float)atof(num);

  memcpy(num, line + 46, 8);
  atom->z = (float)atof(num);
  
  memcpy(num, line + 60, 8);
  atom->temp = (float)atof(num);

  //residue name
  memcpy(resName, line + 17, 3);  
  
  //chaint ID
  chaintID = line[21];
  //residue sequence number
  resSeq = (unsigned)atoi(line + 22);
  return atom;
  
} 
