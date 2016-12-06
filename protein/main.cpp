#include "pdb_parser.h"

int main(){
  PDB_Parser p("test.pdb");
  p.parse();
  
  printf("%d\n", p.getNumbChaints());

  
  Chaint* chaints = p.getChaints();
  for (int i = 0; i < p.getNumbChaints(); i++){
    chaints[i].print();
    
  }
  
  return 0;
}
