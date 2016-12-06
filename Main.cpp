#if defined(WIN32)
#include <windows.h>
#  pragma warning(disable:4244)   // No warnings on precision truncation
#  pragma warning(disable:4305)   // No warnings on precision truncation
#  pragma warning(disable:4786)   // stupid symbol size limitation
#elif defined(UNIX)
#  include <GL/glx.h>
#endif

#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>

#include <GL/glew.h>

#include <GL/glut.h>
#include <glui.h>
#include <GL/glu.h>

//#include <math.h>

#define GLH_EXT_SINGLE_FILE
//#include "glh_extensions.h"
#include "glh_obs.h"
//#include "glh_convenience.h"
//#include "glh_glut.h"
//#include "quitapp.h"

//#define PI 3.15169265
//#define SHADOW_MAP_SIZE 1024
//#define myMin(a, b) ((a < b) ? a : b)

#if !defined(GL_TEXTURE_RECTANGLE_NV) && defined(GL_EXT_texture_rectangle)
#  define GL_TEXTURE_RECTANGLE_NV GL_TEXTURE_RECTANGLE_EXT
#endif

#include "parser.h"
#include "textfile.h"

using namespace std;
using namespace glh;

// glut callbacks
void key(unsigned char k, int x, int y);
void display();
void menu(int entry);
void reshape(int w, int h);
void idle();

void updateLighting(int id);

int setupGLUT(int *argc, char *argv[]);
void setupGLUI(void);
void setShaders();

int glwin;

void idle (void);

// GLUI vars

float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float view_translate[3] = { 0.0, 0.0, 0.0 };
int draw_wireframe = 0;

//int pic_res = 128;

GLUI_Listbox *sur_list;
GLUI_Checkbox *cb2;

int surface_no = 0;
int sh_indi_status = 0;
int coarse_grain_status = 0;
int lighting_status = 0;
int shadow_mapping = 1;
int num_surf;
int use_trans = 0;
int use_peeling;
GLuint occ_query = -1;
int sphere_num = 12;

float min_weight;
float normal_length;

GLfloat light0_ambient[] =  {0.2f, 0.2f, 0.2f, 0.0f};
GLfloat light0_diffuse[] =  {.8f, .8f, .8f, 1.0f};
GLfloat light0_specular[] = {.1f, .1f, .1f, 1.0f};
GLfloat light0_position[] = {0.0, 10.0, 10.0, 0.0};
GLfloat light1_position[] = {0.0, 0.0, 0.0, 1.0};


GLfloat mat_ambient[] = { 0.1, 0.1,  0.1,  1.0};
GLfloat mat_diffuse0[] = { .8,  .8,  .8,  .5};
GLfloat mat_diffuse1[] = { .5,  .0,  .0,  1.0};
GLfloat mat_diffuse2[] = { .0,  .0,  .5,  1.0};
GLfloat mat_diffuse3[] = { .5,  .5,  .0,  1.0};
GLfloat atom_mat_diffuse0[] = { .5,  .5,  .0,  1.0};
GLfloat atom_mat_diffuse1[] = { .0,  .5,  .0,  1.0};
GLfloat atom_mat_diffuse2[] = { .0,  .5,  .5,  1.0};
GLfloat atom_mat_diffuse3[] = { .5,  .5,  .5,  1.0};
GLfloat atom_mat_diffuse4[] = { .5,  .0,  .5,  1.0};
GLfloat atom_mat_diffuse5[] = { .0,  .0,  .0,  1.0};
GLfloat mat_specular[] = { .3,  .3,  .3,  1.0};
GLfloat mat_shininess[] = { 5.0};
GLfloat mat_emission[] = { .1,  .1,  .1,  1.0};
GLfloat no_mat[] = { 0.0,  0.0,  0.0,  1.0};
GLfloat mat_amb_diff[] = { 0.5, 0.5, 0.5, 1.0 };

#define MAX_LAYERS 12

//glut_perspective_reshaper reshaper;
//glut_simple_mouse_interactor camera, object;
tex_object_rectangle  ztex, rgba_layer[MAX_LAYERS];
//bool ztex_consistent = false;

display_list geometry1, geometry2;
arb_fragment_program fp_peel, fp_nopeel;

float alpha = 1.0;
GLuint * zbuf, * zbuf2;
GLenum depth_format;
int layer = 0;
bool peel;

vector<int> surface_list;

GLUquadricObj* myQuad;

myStorage myCoords, myCoords2;

GLhandleARB p;

void initGL()
{
	/*if(! glh_init_extensions("GL_ARB_vertex_program "
                             "GL_ARB_fragment_program "
                             "GL_ARB_fragment_program_shadow "
                             "GL_ARB_depth_texture "
                             "GL_ARB_shadow "))
    {
        cerr << "Necessary extensions were not supported:" << endl
             << glh_get_unsupported_extensions() << endl;
        cerr << "Extensions: " << glGetString(GL_EXTENSIONS) << endl << endl;
        cerr << "Renderer: " << glGetString(GL_RENDERER) << endl << endl;
        quitapp(0);
    }

    if (!glh_extension_supported("GL_EXT_texture_rectangle") && !glh_extension_supported("GL_NV_texture_rectangle"))
    {
        cerr << "Texture rectangles not supported, exiting..." << endl;
        quitapp(0);
    }*/

	ztex.bind();
    ztex.parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    ztex.parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    ztex.parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    ztex.parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    ztex.parameter(GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
    ztex.parameter(GL_TEXTURE_COMPARE_FUNC_ARB, GL_GREATER);

    geometry1.new_list(GL_COMPILE);
		for (int i = 0; i < num_surf; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				//glEnable(GL_BLEND);
				//glDisable(GL_DEPTH_TEST);
				glMaterialfv( GL_FRONT_AND_BACK,  GL_DIFFUSE, mat_diffuse0 );
				glBegin(GL_TRIANGLES);
				glNormal3fv(myCoords[i].centre.normal);
				glVertex3f(myCoords[i].centre.x, myCoords[i].centre.y, myCoords[i].centre.z);
				glNormal3fv(myCoords[i].atoms[j].normal);
				glVertex3f(myCoords[i].atoms[j].x, myCoords[i].atoms[j].y, myCoords[i].atoms[j].z);
				glNormal3fv(myCoords[i].atoms[(j+1)%6].normal);
				glVertex3f(myCoords[i].atoms[(j+1)%6].x, myCoords[i].atoms[(j+1)%6].y, myCoords[i].atoms[(j+1)%6].z);
				glEnd();

				//glDisable(GL_BLEND);
				//glEnable(GL_DEPTH_TEST);
				glMaterialfv( GL_FRONT,  GL_DIFFUSE, mat_diffuse1 );
				glPushMatrix();
				glTranslatef(myCoords[i].centre.x, myCoords[i].centre.y, myCoords[i].centre.z);
				gluSphere(myQuad,1.0, sphere_num, sphere_num);  //draw side chain
				glPopMatrix();

				glBegin(GL_LINES);
				glVertex3f(myCoords[i].centre.x, myCoords[i].centre.y, myCoords[i].centre.z);
				glVertex3f(myCoords[i].sideChain.x, myCoords[i].sideChain.y, myCoords[i].sideChain.z);
				glEnd();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, mat_diffuse2 );
				glPushMatrix();
				glTranslatef(myCoords[i].sideChain.x, myCoords[i].sideChain.y, myCoords[i].sideChain.z);
				gluSphere(myQuad,1.0, sphere_num, sphere_num);  //draw side chain
				glPopMatrix();

			}
		}
	geometry1.end_list();

	geometry2.new_list(GL_COMPILE);
		for (int i = 0; i < myCoords2.size(); i++)
		{
			for (int j = 0; j < 6; j++)
			{
				//glEnable(GL_BLEND);
				//glDisable(GL_DEPTH_TEST);
				glMaterialfv( GL_FRONT_AND_BACK,  GL_DIFFUSE, mat_diffuse0 );
				glBegin(GL_TRIANGLES);
				glNormal3fv(myCoords2[i].centre.normal);
				glVertex3f(myCoords2[i].centre.x, myCoords2[i].centre.y, myCoords2[i].centre.z);
				glNormal3fv(myCoords2[i].atoms[j].normal);
				glVertex3f(myCoords2[i].atoms[j].x, myCoords2[i].atoms[j].y, myCoords2[i].atoms[j].z);
				glNormal3fv(myCoords2[i].atoms[(j+1)%6].normal);
				glVertex3f(myCoords2[i].atoms[(j+1)%6].x, myCoords2[i].atoms[(j+1)%6].y, myCoords2[i].atoms[(j+1)%6].z);
				glEnd();
			}
		}
	geometry2.end_list();

	/*const char peel_fp[] = 
    "!!ARBfp1.0\n"
    "OPTION ARB_fragment_program_shadow;\n"
    "PARAM offset = program.env[0];\n"
    "TEMP R0;\n"
    "MOV R0, fragment.position;\n"
    "ADD R0.x, R0.x, -offset;\n"
    "TEX R0.x, R0, texture[0], SHADOWRECT;\n" 
    "ADD R0.x, R0.x, -0.5;\n"           
    "KIL R0.x;\n"                       
    "MOV result.color, fragment.color;\n"
    "END\n";

    fp_peel.bind();
    fp_peel.load(peel_fp);

    const char nopeel_fp[] = 
    "!!ARBfp1.0\n"
    "MOV result.color, fragment.color.primary;\n"
    "END\n";

	fp_nopeel.bind();
    fp_nopeel.load(nopeel_fp);*/

	//setShaders();

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);

	//glEnable(GL_TEXTURE_2D);
	//glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

	//glGenQueriesARB( 1, &occ_query );  //temp remove
}

/*void loadTexShader(GLuint size, const char * prog_text)
{
        glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, size, prog_text);
		GLint errpos;
		glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
		if(errpos != -1)
		{
			fprintf(stderr, "program error:\n");
			int bgn = errpos - 10;

			const char * c = (const char *)(prog_text + bgn);
			for(int i = 0; i < 30; i++)
			{
				if(bgn+i >= int(size-1))
					break;
				fprintf(stderr, "%c", *c++);
			}
			fprintf(stderr, "\n");
		}
}*/

void setShaders() 
{
	const char peel_fp[] = 
    "!!ARBfp1.0\n"
    "OPTION ARB_fragment_program_shadow;\n"
    "PARAM offset = program.env[0];\n"
    "TEMP R0;\n"
    "MOV R0, fragment.position;\n"
    "ADD R0.x, R0.x, -offset;\n"
    "TEX R0.x, R0, texture[0], SHADOWRECT;\n" 
    "ADD R0.x, R0.x, -0.5;\n"           
    "KIL R0.x;\n"                       
    "MOV result.color, fragment.color;\n"
    "END\n";

	fp_peel.bind();
    fp_peel.load(peel_fp);

	const char nopeel_fp[] = 
    "!!ARBfp1.0\n"
    "MOV result.color, fragment.color.primary;\n"
    "END\n";

	fp_nopeel.bind();
    fp_nopeel.load(nopeel_fp);

	GLhandleARB v,f;
	//h_shadow_vs, h_shadow_fs, h_noshadow_vs;
	char *vs = NULL,*fs = NULL;
	//*shadow_vs = NULL, *shadow_fs, *noshadow_vs = NULL;

	v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	//h_shadow_vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	//h_shadow_fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	//h_noshadow_vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

	vs = textFileRead("lighting.vert");
	fs = textFileRead("lighting.frag");
	//shadow_vs = textFileRead("shadow.vert");
	//shadow_fs = textFileRead("shadow.frag");
	//noshadow_vs = textFileRead("light.vert");

	const char * ff = fs;
	const char * vv = vs;
	//const char * shadow_vs_src = shadow_vs;
	//const char * shadow_fs_src = shadow_fs;
	//const char * noshadow_vs_src = noshadow_vs;

	glShaderSourceARB(v, 1, &vv,NULL);
	glShaderSourceARB(f, 1, &ff,NULL);
	//glShaderSourceARB(h_shadow_vs, 1, &shadow_vs_src, NULL);
	//glShaderSourceARB(h_shadow_fs, 1, &shadow_fs_src, NULL);
	//glShaderSourceARB(h_noshadow_vs, 1, &noshadow_vs_src, NULL);

	free(vs);free(fs); //free(shadow_vs); free(shadow_fs); free(noshadow_vs);

	glCompileShaderARB(v);
	glCompileShaderARB(f);
	//glCompileShaderARB(h_shadow_vs);
	//glCompileShaderARB(h_shadow_fs);
	//glCompileShaderARB(h_noshadow_vs);


	p = glCreateProgramObjectARB();
	glAttachObjectARB(p,f);
	glAttachObjectARB(p,v);

	/*p_shadow = glCreateProgramObjectARB();
	glAttachObjectARB(p_shadow,h_shadow_fs);
	glAttachObjectARB(p_shadow,h_shadow_vs);

	p_noshadow = glCreateProgramObjectARB();
	glAttachObjectARB(p_noshadow,h_noshadow_vs);*/

	glLinkProgramARB(p);
	//glUseProgramObjectARB(p);

	//glLinkProgramARB(p_shadow);
	//glLinkProgramARB(p_noshadow);
	fp_nopeel.bind();  //default
}

int main(int argc, char **argv)
{
	coord_parser* sd = new coord_parser("local_surface.txt");
	sd->compute_normals();
	myCoords = sd->getCoords();

	coord_parser* sd2 = new coord_parser("all_residue.txt");
	sd2->compute_normals();
	myCoords2 = sd2->getCoords();

	setupGLUT(&argc, argv);

	GLint depth_bits;
    glGetIntegerv(GL_DEPTH_BITS, & depth_bits);

	if(depth_bits == 16)  depth_format = GL_DEPTH_COMPONENT16_ARB;
    else                  depth_format = GL_DEPTH_COMPONENT24_ARB;

	GLint alpha_bits;
    glGetIntegerv(GL_ALPHA_BITS, & alpha_bits);
    if (alpha_bits < 8)
    {
        printf("8 bit alpha not supported, exiting...\n");
        exit(-1);
    }

	myQuad = gluNewQuadric();
	num_surf = myCoords.size();

	/*glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("No GLSL support\n");
		exit(1);
	}*/  //temp remove

	initGL();

	//setShaders();  //temp reove

	zbuf = 0;
    zbuf2 = 0;

	//layer = 4;     // start with several layers rendered...
    alpha = .5;    // start with some transparency

	//glut_helpers_initialize();
    //cb.display_function = gldisplay;
    //cb.reshape_function = glreshape;

    //camera.configure_buttons(1);

    //camera.set_camera_mode(true);

    //camera.set_parent_rotation( & camera.trackball.r);

	//glut_add_interactor(& cb);
	//glut_add_interactor(& reshaper);
    //glut_add_interactor(& object);
    //glut_add_interactor(& camera);


    glClearColor(.85,.85,.85,0);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

    setupGLUI();

	glutIdleFunc(idle);
    // GLUT main loop
    glutMainLoop();

    return 0;
}

//
// GLUT callback functions
//

int width, height;
int px, py;

// GL Window callbacks

void render_scene(void)
{
	glPushMatrix();

    glLoadIdentity();

	gluLookAt(0.0, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glTranslatef(view_translate[0], view_translate[1], view_translate[2]);
    glMultMatrixf(view_rotate);
    
    if(draw_wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if ( use_trans == 1)
	{
		glEnable(GL_FRAGMENT_PROGRAM_ARB);

		if(peel)
			fp_peel.bind();
		else
			fp_nopeel.bind();
	}

	glScalef(0.1, 0.1, 0.1);

	/*if (shadow_mapping == 1 && render_mode == 1)
	{
		glGetFloatv(GL_MODELVIEW_MATRIX, tmpModelViewMatrix);
		light->setModelViewMatrix(tmpModelViewMatrix);

		glLoadMatrixf(light->getModelViewMatrix());
	}*/

	//glBeginQueryARB( GL_SAMPLES_PASSED_ARB, occ_query );   //temp remove
    //{
		if (sh_indi_status == 0)
		{
			geometry1.call_list();

			if ( coarse_grain_status == 0)
				geometry2.call_list();
		}
		else
		{
			for (int i = 0; i < surface_list.size(); i++)
			for (int j = 0; j < 6; j++)
			{
				//glEnable(GL_BLEND);
				//glDisable(GL_DEPTH_TEST);
				glMaterialfv( GL_FRONT_AND_BACK,  GL_DIFFUSE, mat_diffuse0 );
				glBegin(GL_TRIANGLES);
				glNormal3fv(myCoords[surface_list[i]].centre.normal);
				glVertex3f(myCoords[surface_list[i]].centre.x, myCoords[surface_list[i]].centre.y, myCoords[surface_list[i]].centre.z);
				glNormal3fv(myCoords[surface_list[i]].atoms[j].normal);
				glVertex3f(myCoords[surface_list[i]].atoms[j].x, myCoords[surface_list[i]].atoms[j].y, myCoords[surface_list[i]].atoms[j].z);
				glNormal3fv(myCoords[surface_list[i]].atoms[(j+1)%6].normal);
				glVertex3f(myCoords[surface_list[i]].atoms[(j+1)%6].x, myCoords[surface_list[i]].atoms[(j+1)%6].y, myCoords[surface_list[i]].atoms[(j+1)%6].z);
				glEnd();

				//glDisable(GL_BLEND);
				//glEnable(GL_DEPTH_TEST);
				glMaterialfv( GL_FRONT,  GL_DIFFUSE, mat_diffuse1 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].centre.x, myCoords[surface_list[i]].centre.y, myCoords[surface_list[i]].centre.z);
				gluSphere(myQuad,1.0, sphere_num, sphere_num);  //draw side chain
				glPopMatrix();

				glBegin(GL_LINES);
				glVertex3f(myCoords[surface_list[i]].centre.x, myCoords[surface_list[i]].centre.y, myCoords[surface_list[i]].centre.z);
				glVertex3f(myCoords[surface_list[i]].sideChain.x, myCoords[surface_list[i]].sideChain.y, myCoords[surface_list[i]].sideChain.z);
				glEnd();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, mat_diffuse2 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].sideChain.x, myCoords[surface_list[i]].sideChain.y, myCoords[surface_list[i]].sideChain.z);
				gluSphere(myQuad,0.7, sphere_num, sphere_num);  //draw side chain
				glPopMatrix();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, atom_mat_diffuse0 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].atoms[0].x, myCoords[surface_list[i]].atoms[0].y, myCoords[surface_list[i]].atoms[0].z);
				gluSphere(myQuad,0.5, sphere_num, sphere_num);  //draw side chain
				glPopMatrix();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, atom_mat_diffuse0 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].atoms[0].x, myCoords[surface_list[i]].atoms[0].y, myCoords[surface_list[i]].atoms[0].z);
				gluSphere(myQuad,0.5, sphere_num, sphere_num); 
				glPopMatrix();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, atom_mat_diffuse1 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].atoms[1].x, myCoords[surface_list[i]].atoms[1].y, myCoords[surface_list[i]].atoms[1].z);
				gluSphere(myQuad,0.5, sphere_num, sphere_num);
				glPopMatrix();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, atom_mat_diffuse2 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].atoms[2].x, myCoords[surface_list[i]].atoms[2].y, myCoords[surface_list[i]].atoms[2].z);
				gluSphere(myQuad,0.5, sphere_num, sphere_num);
				glPopMatrix();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, atom_mat_diffuse3 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].atoms[3].x, myCoords[surface_list[i]].atoms[3].y, myCoords[surface_list[i]].atoms[3].z);
				gluSphere(myQuad,0.5, sphere_num, sphere_num);
				glPopMatrix();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, atom_mat_diffuse4 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].atoms[4].x, myCoords[surface_list[i]].atoms[4].y, myCoords[surface_list[i]].atoms[4].z);
				gluSphere(myQuad,0.5, sphere_num, sphere_num);  //draw side chain
				glPopMatrix();

				glMaterialfv( GL_FRONT,  GL_DIFFUSE, atom_mat_diffuse5 );
				glPushMatrix();
				glTranslatef(myCoords[surface_list[i]].atoms[5].x, myCoords[surface_list[i]].atoms[5].y, myCoords[surface_list[i]].atoms[5].z);
				gluSphere(myQuad,0.5, sphere_num, sphere_num);  //draw side chain
				glPopMatrix();
			}
		}
	//}
	//glEndQueryARB( GL_SAMPLES_PASSED_ARB );

	glPopMatrix();

	if (use_trans == 1)
		glDisable(GL_FRAGMENT_PROGRAM_ARB);

    //glutSwapBuffers();
}

void render_scene_from_camera_view(bool shadow, bool update_ztex, int l)
{
    peel = shadow;
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glLoadIdentity();
    //camera.apply_inverse_transform();
    ztex.bind();
    render_scene();

    glPopMatrix();

    if(update_ztex)
    {
        // copy the z
        ztex.bind();
        glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, width, height);
    }

    // copy the RGBA of the layer
    rgba_layer[l].bind();
    glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, width, height);
    glFinish();
    glFinish();
}

// draw a textured quad for each layer and blend them all together correctly
void draw_sorted_transparency()
{
    int w = width;
    int h = height;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
	//gluPerspective(60.0, w/h, 1.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);

    for(int i=layer; i >= 0; i--)
    {
        rgba_layer[i].bind();
        rgba_layer[i].enable();
        
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0);
        glVertex2f(0, 0);
        glTexCoord2f(0, h);
        glVertex2f(0, h);
        glTexCoord2f(w, h);
        glVertex2f(w, h);
        glTexCoord2f(w, 0);
        glVertex2f(w, 0);
        glEnd();      
    }
    rgba_layer[0].disable();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void bad_transparency()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glDisable(GL_DEPTH_TEST);
    render_scene_from_camera_view(false, false, 0);
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
}

void good_transparency()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

	GLuint occ_query_count;
	GLuint init_count;

    for(int l=0; l <= MAX_LAYERS; l++)
	{
		render_scene_from_camera_view(l > 0, l < MAX_LAYERS, l);
		glGetQueryObjectuivARB( occ_query, GL_QUERY_RESULT_ARB, &occ_query_count);
		
		layer = l;

		if ( l == 0)
			init_count = occ_query_count;

		if (occ_query_count < init_count * 0.2 || layer >= 4)
			break;
	}

    draw_sorted_transparency();
}

void display(void)
{
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  //render_scene();
  if (lighting_status == 0)
  {
	  if (use_trans == 1)
	  {
		  if (use_peeling == 0)
			good_transparency();
		  else if (use_peeling == 1)
			bad_transparency();
	  }
	  else
		  render_scene();
  }
  else
	render_scene();

  //glFlush();
  glFlush();
  glutSwapBuffers();
}

// react to size changes
void reshape(int w, int h)
{
    //int tx, ty, tw, th;
    GLUI_Master.get_viewport_area( &px, &py, &width, &height );
    glViewport( px, py, width, height);

    glMatrixMode(GL_PROJECTION);  
    glLoadIdentity();

	//glOrtho(-2.0, 2.0, -2.0, 2.0, -5.0, 5.0);
	gluPerspective(60.0, width/height, 1.0, 15.0);
 
    glMatrixMode(GL_MODELVIEW);
    
	//if (use_trans == 1)
	//{
		/*ztex.bind();
		delete [] zbuf;
		delete [] zbuf2;
		zbuf = new GLuint [ width * height ];
		zbuf2 = new GLuint [ width * height ];

		glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, depth_format, width, height, 0, 
                GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, zbuf);

		for(int i=0; i < MAX_LAYERS; i++)
		{
			rgba_layer[i].bind();
			glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA8, width, height, 0, 
				GL_RGBA, GL_UNSIGNED_BYTE, zbuf);
			rgba_layer[i].parameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			rgba_layer[i].parameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		delete [] zbuf;
		delete [] zbuf2;
		zbuf = new GLuint [ width * height ];
		zbuf2 = new GLuint [ width * height ];

		glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, 0, 0, 0, 0, 0);*/  //temp remove
	//}

    //glutPostRedisplay();
}

void idle( void )
{
  //light->update();

  if(glutGetWindow() != glwin) 
    glutSetWindow(glwin);  
    
  glutPostRedisplay(); 
}

// react to keys
void key(unsigned char k, int x, int y)
{
  switch(k)
  {
    case 27:
      exit(1);
      break;
    default:
        glutPostRedisplay();
        break;
  }  
  return;    
}

void updateList(int id)
{
  if (sh_indi_status == 0)
  {
		sur_list->disable();
		cb2->enable();
  }
  else
  {
		sur_list->enable();
		cb2->disable();
  }
}

void updateLighting(int id)
{
	if (lighting_status == 0)
		glUseProgramObjectARB(0);
	else
		glUseProgramObjectARB(p);
}

void add_surface(int id)
{
	surface_list.push_back(surface_no);
}

void clear_surface_list(int id)
{
	surface_list.clear();
}

// setup the GLUT library which handles the windows for us
int setupGLUT(int *argc, char *argv[])
{
    glutInit(argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    // The Ray Tracing Window

    /*glutInitWindowSize(600, 600);

    rtwin = glutCreateWindow("demo");
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);*/
     
    // The GL window, with GLUI interface    
    glutInitWindowSize(800,600);
	glutInitDisplayString("alpha>=8 rgb double depth>=24");
    glwin = glutCreateWindow("GL Scene");
    
    glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	//glutKeyboardFunc(keyboard);
	//glutIdleFunc(idle);

    GLUI_Master.set_glutReshapeFunc( reshape );  
    GLUI_Master.set_glutKeyboardFunc( key );
	GLUI_Master.set_glutIdleFunc(idle);
//    GLUI_Master.set_glutSpecialFunc( NULL );
//    GLUI_Master.set_glutMouseFunc( NULL );

    return glwin;
}

void setupGLUI(void)
{
    // Setup GLUI
    GLUI *gluib = GLUI_Master.create_glui_subwindow(glwin,
            GLUI_SUBWINDOW_BOTTOM);
    gluib->set_main_gfx_window( glwin );

    // Somehow add_rotation screws up the matrix. Save it.
    float mcopy[16];
    memcpy(mcopy, view_rotate, sizeof(float) * 16);
    
    GLUI_Rotation *view_rot = gluib->add_rotation( "Scene", view_rotate );
    view_rot->set_spin( 1.0 );
    gluib->add_column( false );
    
    memcpy(view_rotate, mcopy, sizeof(float) * 16);

    GLUI_Translation *trans_xy = 
    gluib->add_translation( "View XY", GLUI_TRANSLATION_XY, view_translate );
    trans_xy->set_speed( .005 );
    trans_xy->set_x(view_translate[0]);
    trans_xy->set_y(view_translate[1]);
    gluib->add_column( false );
    GLUI_Translation *trans_x = 
    gluib->add_translation( "View X", GLUI_TRANSLATION_X, &view_translate[0] );
    trans_x->set_speed( .005 );
    trans_x->set_x(view_translate[0]);
    gluib->add_column( false );
    GLUI_Translation *trans_y = 
    gluib->add_translation( "View Y", GLUI_TRANSLATION_Y, &view_translate[1] );
    trans_y->set_speed( .005 );
    trans_y->set_y(view_translate[1]);
    gluib->add_column( false );
    GLUI_Translation *trans_z = 
    gluib->add_translation( "View Z", GLUI_TRANSLATION_Z, &view_translate[2] );
    trans_z->set_speed( .005 );
    trans_z->set_z(view_translate[2]);
    gluib->add_column( false );
    
    gluib->add_checkbox( "Wireframe", &draw_wireframe );

    // Setup GLUI
    GLUI *glui = GLUI_Master.create_glui_subwindow(glwin, GLUI_SUBWINDOW_RIGHT);
    glui->set_main_gfx_window( glwin );

	GLUI_Panel *panel1;
	panel1 = glui->add_panel("Show individual Surface");

	GLUI_Checkbox *cb1;
	cb1 = glui->add_checkbox_to_panel(panel1, "show individual surface:", &sh_indi_status, sh_indi_status, updateList);
	//cb->set_int_val(sh_indi_status);

    //GLUI_Listbox *list;
	sur_list = glui->add_listbox_to_panel(panel1, "Show surface:", &surface_no, 0, add_surface);
	glui->add_button_to_panel(panel1, "Clear", 0, clear_surface_list );

	GLUI_Panel *panel2;
	panel2 = glui->add_panel("Show coarse-grain surface only");

	//GLUI_Checkbox *cb2;
	cb2 = glui->add_checkbox_to_panel(panel2, "show coarse-grain surface only", &coarse_grain_status);

	if (sh_indi_status == 0)
	{
		sur_list->disable();
		cb2->enable();
	}

	GLUI_Panel *panel3;
	panel3 = glui->add_panel("Per-pixel lighting");

	GLUI_Checkbox *cb3;
	cb3 = glui->add_checkbox_to_panel(panel3, "Enable per-pixel lighting", &lighting_status, 0, updateLighting);

	GLUI_Panel *panel4;
	panel4 = glui->add_panel("Transparency");

	GLUI_Checkbox *cb4;
	cb4 = glui->add_checkbox_to_panel(panel4, "Use transparency", &use_trans);
	
	GLUI_RadioGroup* trans_rg = glui->add_radiogroup_to_panel(panel4, &use_peeling);
	GLUI_RadioButton *rb1;
	GLUI_RadioButton *rb2;
	rb1 = glui->add_radiobutton_to_group(trans_rg, "with depth peeling");
	rb1 = glui->add_radiobutton_to_group(trans_rg, "without depth peeling");

	if (sh_indi_status == 0)
	{
		sur_list->disable();
		cb2->enable();
	}

	char sur_label[10];

	for (int i = 0; i < myCoords.size(); i++)
	{
		sprintf(sur_label, "%d.%s", i, myCoords[i].label);
		sur_list->add_item(i, sur_label);
	}

	sur_list->set_int_val(surface_no);

	glui->add_button( "Quit", 0, (GLUI_Update_CB)exit );

//    GLUI_Master.set_glutIdleFunc( NULL );
}
