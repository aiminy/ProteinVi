#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <GL/glut.h>
//#include <glui.h>
//#include <GL/glu.h>

using namespace std;

struct myPoint
{
	double x;
	double y;
	double z;

	GLfloat normal[3];
};

struct mySurface
{
	char label[3];

	myPoint centre;
	myPoint sideChain;
	myPoint atoms[6];
};

typedef vector<mySurface> myStorage;

class coord_parser
{
public:
	coord_parser(char* fname);
	~coord_parser();

	myStorage getCoords();
	void compute_normals();

private:
	myStorage localCoords;

};