#ifndef __INFO
#define __INFO

enum AMINO_ACID {ALA = 0, ARG, ASN, ASP, CYS, GLN, GLU, GLY, HIS, ILE,
		 LEU, LYS, MET, PHE, PRO, SER, THR, TRP, TYR, VAL, UNKOWN};

//0 uncharged, 1 positively charge, 2 negatively charged
static char aa_charge[21]={
  0, 1, 0, 2, 0, 0, 2, 0, 1, 0,
  0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static float aa_charge_color[3][3]={
  {.5, .5, .5},
  {1, 0, 0},
  {0, 0, 1}
};

//0 nonpolar, 1 polar, 2 ionized
static char aa_polar[21]={
  0, 2, 1, 2, 0, 1, 2, 1, 2, 0,
  0, 2, 0, 0, 0, 1, 1, 0, 1, 0, 0
};

static float aa_polar_color[3][3]={
  {1, 1, 1},
  {1, 1, 0},
  {0, 1, 1}
};

/* aa name*/
static char* aa_names[20] = {"ALA","ARG","ASN","ASP","CYS","GLN","GLU","GLY","HIS","ILE",

			     "LEU","LYS","MET","PHE","PRO","SER","THR","TRP","TYR","VAL"};

/* number of atoms in aa*/
static char aa_numb_atoms[20] ={5, 11, 8, 8, 6, 9, 9, 4, 10, 8,
				8, 9, 8, 11, 7, 6, 7, 14,12, 7};	 

/* atom names in aa */
static char* aa_atoms[20][15]={{"N", "C", "CA", "O", "CB", 0},                                    //ala 1
			       {"N", "C", "CA", "O", "CB", "CG", "CD", "NE", "CZ", "NH1", "NH2", 0},//arg 2
			       {"N", "C", "CA", "O", "CB", "CG", "OD1", "ND2", 0},                   //asn 3
			       {"N", "C", "CA", "O", "CB", "CG", "OD1", "OD2", 0},                //asp 4
			       {"N", "C", "CA", "O", "CB", "SG", 0},                              //cyc 5
			       {"N", "C", "CA", "O", "CB", "CG", "CD", "OE1", "NE2", 0},          //gln 6
			       {"N", "C", "CA", "O", "CB", "CG", "CD", "OE1", "OE2", 0},          //glu 7
			       {"N", "C", "CA", "O", 0},                                          //gly 8
			       {"N", "C", "CA", "O", "CB", "CG", "ND1", "CD2", "CE1", "NE2", 0},  //his 9
			       {"N", "C", "CA", "O", "CB", "CG2", "CG1", "CD1", 0},               //ile 10
			       {"N", "C", "CA", "O", "CB", "CG", "CD1", "CD2", 0},                //leu 11
			       {"N", "C", "CA", "O", "CB", "CG", "CD", "CE", "NZ", 0},            //lys 12
			       {"N", "C", "CA", "O", "CB", "CG", "SD", "CE", 0},                  //met 13
			       {"N", "C", "CA", "O", "CB", "CG", "CD1", "CD2", "CE1", "CE2", "CZ", 0}, //phe 14
			       {"N", "C", "CA", "O", "CB", "CG", "CD", 0},                        //pro 15
			       {"N", "C", "CA", "O", "CB", "OG", 0},                              //ser 16
			       {"N", "C", "CA", "O", "CB", "OG1", "CG2", 0},                      //thr 17
			       {"N", "C", "CA", "O", "CB", "CG", "CD1", "CD2", "NE1", "CE2", 
				"CE3", "CZ2", "CZ3", "CH2", 0},                                   //trp 18
			       {"N", "C", "CA", "O", "CB", "CG", "CD1", "CD2", "CE1", "CE2",
				"CZ", "OH", 0},                                                   //tyr 19
			       {"N", "C", "CA", "O", "CB", "CG1", "CG2", 0}                       //val 20
};

static char aa_bonds[20][20][3]={{{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {-1, -1, -1}},                                                              //ala
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {6,7,1}, {7,8,1}, {8,9,1}, {8,10,1}, {-1, -1, -1}},             //arg
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,2}, {5,7,1}, {-1, -1, -1}},                                      //asn
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,2}, {5,7,1}, {-1, -1, -1}},                                      //asp
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {-1, -1, -1}},                                                      //cys
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {6,7,2}, {6,8,1}, {-1, -1, -1}},                              //gln
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {6,7,2}, {6,8,1}, {-1, -1, -1}},                              //glu
				 {{0,2,1}, {2,1,1}, {1,3,2}, {-1, -1, -1}},                                                                      //gly
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {5,7,2}, {6,8,1}, {7,9,1}, {8,9,1}, {-1, -1, -1}},              //his
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,6,1}, {4,5,1}, {6,7,1}, {-1, -1, -1}},                                      //ile
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {5,7,1}, {-1, -1, -1}},                                      //leu
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {6,7,1}, {7,8,1}, {-1, -1, -1}},                              //lys
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {6,7,1}, {-1, -1, -1}},                                      //met
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,2}, {5,7,1}, {6,8,1}, {7,9,2}, {8,10,1}, {9,10,2}, {-1, -1, -1}},   //phe
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {6,0,1}, {-1, -1, -1}},                                      //pro
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {-1, -1, -1}},                                                      //ser
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {4,6,1}, {-1, -1, -1}},                                              //thr
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,2}, {5,6,1}, {6,8,1}, {7,9,2}, {7,10,1},              //trp
				  {8,9,1}, {9,11,2}, {10,12,2}, {11,13,2}, {12,13,1}, {-1, -1, -1}},
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {5,6,1}, {5,7,2}, {6,8,2}, {7,9,1}, {8,10,1}, {9,10,2}, {10,11,1}, {-1, -1, -1}},  //tyr
				 {{0,2,1}, {2,1,1}, {1,3,2}, {2,4,1}, {4,5,1}, {4,6,1}, {-1, -1, -1}}                                               //val
};

static char *atom_names[]={  
  "H",         // Hydrogen
  "He",        // Helium
  "Li",        // Lithium
  "Be",        // Beryllium
  "B",         // Boron
  "C",         // Carbon
  "N",         // Nitrogen
  "O",         // Oxygen
  "F",         // Fluorine
  "Ne",        // Neon
  "Na",        // Sodium
  "Mg",        // Manganese
  "Al",        // Aluminum
  "Si",        // Silicon
  "P",         // Phosphorus
  "S",         // Sulfur
  "Cl",        // Chlorine
  "Ar",        // Argon
  "K",         // Potassium
  "Ca",        // Calcium
  "Sc",        // Scandium
  "Ti",        // Titanium
  "V",         // Vanadium
  "Cr",        // Chromium
  "Mn",        // Manganese
  "Fe",        // Iron
  "Co",        // Cobalt
  "Ni",        // Nickle
  "Cu",        // Copper
  "Zn",        // Zinc
  "Ga",        // Gallium
  "Ge",        // Germanium
  "As",        // Arsenic
  "Se",        // Selenium
  "Br",        // Boron
  "Kr"         //Krypton
};

static float atom_sizes[] = { // sizes increase as the electron shells increase
	0.37,		// Hydrogen
	0.50,		// Helium
	1.52,		// Lithium
	1.11,		// Beryllium
	0.88,		// Boron
	0.77,		// Carbon
	0.70,		// Nitrogen
	0.66,		// Oxygen
	0.64,		// Fluorine
	0.70,		// Neon
	1.5, // Sodium *
	1.5, // Manganese *
	1.5, // Aluminum *
	1.17,		// Silicon
	1.10,		// Phosphorus
	1.5, // Sulfur
	0.99,		// Chlorine
	1.5, // Argon *
	2.0, // Potassium *
	2.0, // Calcium *
	2.0, // Scandium *
	2.0, // Titanium *
	2.0, // Vanadium *
	2.0, // Chromium *
	2.0, // Manganese *
	2.0, // Iron *
	2.0, // Cobalt *
	2.0, // Nickle *
	2.0, // Copper *
	2.0, // Zinc *
	2.0, // Gallium *
	2.0, // Germanium *
	2.0, // Arsenic *
	2.0, // Selenium *
	2.0, // Boron *
	2.0  // Krypton *
};


#endif
