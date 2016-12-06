#include "parser.h"
#include <math.h>

myPoint crossProd(myPoint v1, myPoint v2)
{
	myPoint ret;

	ret.x = v1.y * v2.z - v1.z * v2.y;
	ret.y = v1.z * v2.x - v1.x * v2.z;
	ret.z = v1.x * v2.y - v1.y * v2.x;

	return ret;
}

myPoint normalize(myPoint v)
{
	myPoint ret;

	double local_sum = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);

	ret.x = v.x/local_sum;
	ret.y = v.y/local_sum;
	ret.z = v.z/local_sum;

	return ret;
}

coord_parser::coord_parser(char* fname)
{
	char buffer[100];
	char buffer2[100];
	char *n1, *n2;

	char* coord1;
	char* coord2;
	char* coord3;

	int lineNo =-1;
	mySurface* aSurface;

	ifstream inFile(fname);

	while (!inFile.eof())
	{
			inFile.getline(buffer, 100);
			lineNo++;

			if((lineNo-1)%9 == 0)
			{
				aSurface = (mySurface*)malloc(sizeof mySurface);
				sprintf(aSurface->label, "%s", buffer);
			}

			n1 = strchr(buffer, '(');
			n2 = strchr(buffer, ')');

			if (n1 != NULL && n2 != NULL)
			{
				strncpy(buffer2, n1+1, n2-n1-1);
				buffer2[(int)(n2-n1-1)] = '\0';

				coord1 = strtok(buffer2, ",");
				coord2 = strtok(NULL, ",");
				coord3 = strtok(NULL, ",");

				if (strstr(buffer, "Ca"))
				{
					aSurface->centre.x = atof(coord1);
					aSurface->centre.y = atof(coord2);
					aSurface->centre.z = atof(coord3);
				}
				else if (strstr(buffer, "GcSc"))
				{
					aSurface->sideChain.x = atof(coord1);
					aSurface->sideChain.y = atof(coord2);
					aSurface->sideChain.z = atof(coord3);
				}
				else if (strstr(buffer, "FnAtom"))
				{
					aSurface->atoms[0].x = atof(coord1);
					aSurface->atoms[0].y = atof(coord2);
					aSurface->atoms[0].z = atof(coord3);
				}
				else if (strstr(buffer, "SnAtom"))
				{
					aSurface->atoms[1].x = atof(coord1);
					aSurface->atoms[1].y = atof(coord2);
					aSurface->atoms[1].z = atof(coord3);
				}
				else if (strstr(buffer, "Atom_3rd"))
				{
					aSurface->atoms[2].x = atof(coord1);
					aSurface->atoms[2].y = atof(coord2);
					aSurface->atoms[2].z = atof(coord3);
				}
				else if (strstr(buffer, "Atom_4th"))
				{
					aSurface->atoms[3].x = atof(coord1);
					aSurface->atoms[3].y = atof(coord2);
					aSurface->atoms[3].z = atof(coord3);
				}
				else if (strstr(buffer, "Atom_5th"))
				{
					aSurface->atoms[4].x = atof(coord1);
					aSurface->atoms[4].y = atof(coord2);
					aSurface->atoms[4].z = atof(coord3);
				}
				else if (strstr(buffer, "Atom_6th"))
				{
					aSurface->atoms[5].x = atof(coord1);
					aSurface->atoms[5].y = atof(coord2);
					aSurface->atoms[5].z = atof(coord3);
				}
			}

			if((lineNo-1)%9 == 8)
				localCoords.push_back(*aSurface);

	}

	inFile.close();
}

void coord_parser::compute_normals()
{
	double surface_norm[6][3];
	double tmp_total_norm[3];

	myPoint vec1, vec2;
	myPoint tmp_norm;

	for (int i = 0; i < 3; i++)
		tmp_total_norm[i] = 0.0;

	for (int i = 0; i < localCoords.size(); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			vec1.x = localCoords[i].atoms[j].x - localCoords[i].centre.x;
			vec1.y = localCoords[i].atoms[j].y - localCoords[i].centre.y;
			vec1.z = localCoords[i].atoms[j].z - localCoords[i].centre.z;

			vec2.x = localCoords[i].atoms[(j+1)%6].x - localCoords[i].centre.x;
			vec2.y = localCoords[i].atoms[(j+1)%6].y - localCoords[i].centre.y;
			vec2.z = localCoords[i].atoms[(j+1)%6].z - localCoords[i].centre.z;

			tmp_norm = normalize(crossProd(vec1, vec2));

			surface_norm[j][0] = tmp_norm.x;
			surface_norm[j][1] = tmp_norm.y;
			surface_norm[j][2] = tmp_norm.z;
		}

		for (int j = 1; j < 7; j++)
		{
			localCoords[i].atoms[j-1].normal[0] = (surface_norm[j-1][0] + surface_norm[j][0])/2;
			localCoords[i].atoms[j-1].normal[1] = (surface_norm[j-1][1] + surface_norm[j][1])/2;
			localCoords[i].atoms[j-1].normal[2] = (surface_norm[j-1][2] + surface_norm[j][2])/2;

			tmp_total_norm[0] += surface_norm[j-1][0];
			tmp_total_norm[1] += surface_norm[j-1][1];
			tmp_total_norm[2] += surface_norm[j-1][2];
		}

		localCoords[i].centre.normal[0] = tmp_total_norm[0]/6;
		localCoords[i].centre.normal[1] = tmp_total_norm[1]/6;
		localCoords[i].centre.normal[2] = tmp_total_norm[2]/6;
	}
}





vector<mySurface> coord_parser::getCoords()
{
	return localCoords;
}

