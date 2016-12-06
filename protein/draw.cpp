#include "pdb_parser.h"

void PDB_Parser::draw(){
  glPushMatrix();
  glInitNames();
  glPushName(0);
  //centering
  GLfloat centroid[3] = {0, 0 ,0};
  for (int i = 0; i < getNumbChaints(); i++){
    GLfloat* cur = getChaint(i)->getCentroid();
    centroid[0] += cur[0];
    centroid[1] += cur[1];
    centroid[2] += cur[2];
  }
  centroid[0] /= getNumbChaints();
  centroid[1] /= getNumbChaints();
  centroid[2] /= getNumbChaints();
  glTranslatef(-centroid[0], -centroid[1], -centroid[2]);
  //draw chaints
  for (int i = 0; i < getNumbChaints(); i++){
    Chaint* chaint = getChaint(i);
    glLoadName(i + 1);
    chaint->draw(_mode);    
    //glPopName();
  }
  //glPopName(1);
  glPopMatrix();
}

void AminoAcid::setColor(int mode, pdb_atom* atom){
  
  if (mode & COLOR_MODE_ATOM_TYPE && atom){
    switch (atom->name[0]){
    case 'H': glColor3f(0, 1, 1); break;
    case 'N': glColor3f(.7, 1, 1); break;
    case 'O': glColor3f(0, 0, 1); break;
    case 'C': glColor3f(.1, .1, .1); break;
    case 'S': glColor3f(1, 1, 0); break;
    }
    return;
  }

  if (mode & COLOR_MODE_ATOM_TEMP && atom){
    glColor3f(atom->temp / 100, .3, 1 - atom->temp/100);
    return;
  }
  
  if (mode &  COLOR_MODE_AA_POLAR) {
    glColor3fv(aa_polar_color[aa_polar[_type]]);
    return;
  }
  
  if (mode & COLOR_MODE_AA_CHARGE) {
    glColor3fv(aa_charge_color[aa_charge[_type]]);
    return;
  }
  
}

void AminoAcid::draw(int mode){
  if (mode & DRAW_MODE_ATOMS){
    glEnable(GL_LIGHTING);
    for (int i = 0; i < aa_numb_atoms[_type]; i++){
      if (mode & DRAW_MODE_NO_SIDE_CHAINTS && i >= 3) break;
      double radius= .3;
      pdb_atom *atom = _standard_atoms[i];
      switch (atom->name[0]){
      case 'H': radius = .37; break;
      case 'N': radius = .70; break;
      case 'O': radius = .66; break;
      case 'C': radius = .77; break;
      case 'S': radius = 1.5; break;
      }
      setColor(mode, atom);
      glPushMatrix();
      glTranslatef(atom->x, atom->y, atom->z);
      if (mode & DRAW_MODE_BONDS) 
	glScalef(.6, .6, .6);
      glutSolidSphere(radius, 10, 6);
      //glutSolidTeapot(radius);
      glPopMatrix();
    }
  }
  if (mode & DRAW_MODE_CHAINT_VECTOR){
    glDisable(GL_LIGHTING);
    calculateCentroid();
    setColor(mode, _standard_atoms[1]);
    glBegin(GL_LINES);
    glVertex3fv(_centroid);
    glColor3f(1, 0, 0);
    glVertex3fv((GLfloat*)_standard_atoms[1]);
    glEnd();
  }
  if (mode & DRAW_MODE_BONDS){
    glDisable(GL_LIGHTING);
    int index = 0;
    while(index <= 20){
      char* bounds = aa_bonds[_type][index];
      if (bounds[0] < 0) break;
      if (!_standard_atoms[bounds[0]] || !_standard_atoms[bounds[1]]) {
	index ++;
	continue;
      } 
      if (mode & DRAW_MODE_NO_SIDE_CHAINTS && index >= 2) break;
      if (bounds[2] <= 1){
	glLineWidth(2);
      }
      else{
	glLineWidth(3);
      }
      
      glBegin(GL_LINES);
      glVertex3fv((GLfloat*)_standard_atoms[bounds[0]]);
      glVertex3fv((GLfloat*)_standard_atoms[bounds[1]]);
      glEnd();
      index++;
    }
  }
}

void Chaint:: draw(int mode){
  glPushMatrix();  
  for (unsigned int i = 0; i < _aa.size(); i++){
    glColor3fv(_color);
    _aa[i].draw(mode);
    if (mode & DRAW_MODE_BONDS && i < _aa.size() - 1){
      AminoAcid& cur = _aa[i];
      AminoAcid& next = _aa[i + 1];
      glLineWidth(2);
      glBegin(GL_LINES);
      glVertex3fv((GLfloat*)(cur.getAtoms()[1]));
      glVertex3fv((GLfloat*)(next.getAtoms()[0]));
      glEnd();
    } 
  }
  if (mode & DRAW_MODE_CURVES){

#if 1
    glDisable(GL_LIGHTING);
    glLineWidth(2);
    glColor3fv(_color);
    GLfloat ctrlPoints[_aa.size() * 3][3];
    for (unsigned int i = 0; i < _aa.size(); i++){
      pdb_atom* atom1 = _aa[i].get(0);
      pdb_atom* atom2 = _aa[i].get(2);
      pdb_atom* atom3 = _aa[i].get(1);
      memcpy(ctrlPoints[i * 3 + 0], atom1, sizeof(GLfloat) * 3);
      memcpy(ctrlPoints[i * 3 + 1], atom2, sizeof(GLfloat) * 3);
      memcpy(ctrlPoints[i * 3 + 2], atom3, sizeof(GLfloat) * 3);
    }
    glMap1f(GL_MAP1_VERTEX_3, 0, 10, 3, _aa.size() * 3, &ctrlPoints[0][0]);
    glEnable(GL_MAP1_VERTEX_3);
    //glColor3f(.5, .5, 0);
    glBegin(GL_LINE_STRIP);
    for (float i = 0; i < 10; i += .1f){
      glEvalCoord1f(i);
    }
    glEnd();    
#endif
    glEnable(GL_LIGHTING);
  }
  glPopMatrix();
}

GLfloat* Chaint::getCentroid(){
  if (_centroid[0] == 0 &&
      _centroid[1] == 0 &&
      _centroid[2] == 0){
   
    int count = 0;
    for (unsigned int i = 0; i < _aa.size(); i++){
      AminoAcid& aa = _aa[i];
      pdb_atom** atoms = aa.getAtoms();
      for (int j = 0; j < aa.getNumbAtoms(); j++){
	if (atoms[j]){
	  count++;
	  _centroid[0] += atoms[j]->x;
	  _centroid[1] += atoms[j]->y;
	  _centroid[2] += atoms[j]->z;
	}
      }
    }
    
    _centroid[0] /= count;
    _centroid[1] /= count;
    _centroid[2] /= count;
  }
  return _centroid;
}
