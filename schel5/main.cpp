#include <iostream>
#include<windows.h>
#include <GL/freeglut.h>

using namespace std;

GLuint texture = 0;
GLdouble left_m = -100.0;
GLdouble right_m = 700.0;
GLdouble bottom_m = -140.0;
GLdouble top_m = 460.0;
double ok = 1;
double j = 0.0;
double i = 0.0;
double contor = 0;
double loc_vert = 460; // loc_vert e de fapt loc_oriz, adica punctul pe orizontala unde pleaca toti strugurii
int vector[3] = { 0, 200, 400 };
double height = vector[rand() % 3]; // height va fi de fapt pozitia pe orizontala a cosului care prinde struguri
int score = 0;
double timp = 0.15;
int pct = 1000;
double rsj, rdj, rss, rds = 0;
int vieti = 3;


void init(void)
{
	glClearColor(0.98, 0.929, 0.792, 0.0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(left_m, right_m, bottom_m, top_m, -1.0, 1.0);
}

void RenderString(float x, float y, void* font, const unsigned char* string)
{

	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2f(x, y);

	glutBitmapString(font, string);
}


void background(GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
	glEnd();
}

void startgame(void)
{

	if (vieti > 0) {

		if (i < -380)
		{
			i = 0;
		}
		i = i - 2 * timp;
		loc_vert -= timp;


		//daca cosul e pe aceeasi coloana cu strugurele si in intervalul ..., atunci creste scorul
		if (height == j) {

			if (loc_vert > -100 && loc_vert < -50) {
				score += 100;
				height = vector[rand() % 3];
				cout << "Score:  " << score << endl;
				cout << "Nr vieti: " << vieti << endl;
				loc_vert = 460;
			}
		}

		if (loc_vert < -100) {
			vieti--;
			loc_vert = 460;
			height = vector[rand() % 3];
		}

	}
	else {
		ok = 0;
	}


}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	background(texture);

	glColor3f(0.55, 0.788, 0.451);

	// Iarba de jos
	glBegin(GL_POLYGON);
	glVertex2i(-100, -140);// Stanga jos
	glVertex2i(700, -140); // Dreapta jos
	glVertex2i(700, -80);  // Dreapta sus
	glVertex2i(-100, -80); // Stanga sus
	glEnd();

	// Iarba de sus
	glBegin(GL_POLYGON);
	glVertex2i(-100, 400);// Stanga jos
	glVertex2i(700, 400); // Dreapta jos
	glVertex2i(700, 460); // Dreapta sus
	glVertex2i(-100, 460);// Stanga sus
	glEnd();
	RenderString(200.0f, 425.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Prinde strugurii!");

	// Delimitare sosea
	glLineWidth(3);
	glColor3f(1, 1, 1);

	// Delimitam soseaua de iarba partea de jos
	glBegin(GL_LINES);
	glVertex2i(-100, -80);
	glVertex2i(1500, -80);
	glEnd();

	// Delimitam soseaua de iarba partea de sus
	glBegin(GL_LINES);
	glVertex2i(-100, 400);
	glVertex2i(1500, 400);
	glEnd();

	// Liniile intrerupte
	glPushMatrix();
	glTranslated(0.0, 0.0, 0.0);


	glBegin(GL_LINES);
	glVertex2i(100, -80);
	glVertex2i(100, 400);
	glEnd();

	glBegin(GL_LINES);
	glVertex2i(300, -80);
	glVertex2i(300, 400);
	glEnd();
	glPopMatrix();



	//desenam masina
	glPushMatrix();
	glTranslated(j, -50.0, 0.0);



	glColor3f(0.996, 0.365, 0.149);
	glRecti(-45, -15, 45, 15);

	if (ok == 0)
	{
		rsj = 8;
		rss = -8;
		rdj = -8;
		rds = 8;
	}


	glPopMatrix();
	glPopMatrix();

	if (ok == 0) {
		RenderString(250.0f, 200.0f, GLUT_BITMAP_8_BY_13, (const unsigned char*)"GAME OVER");
	}

	if (contor == 1 && (j != 200 && j != 400))
		j = j + 1;
	else if (contor == -1 && (j != 200 && j != 0))
		j = j - 1;
	else {
		contor = 0;
	}

	//desenam a doua masina (adversara)
	glPushMatrix();
	glTranslated(height, loc_vert, 0.0);

	glColor3f(0.471, 0.667, 0.949);
	glRecti(-15, -45, 15, 45);


	glPopMatrix();

	startgame();
	glutPostRedisplay();
	glutSwapBuffers();
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left_m, right_m, bottom_m, top_m, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void miscast(void)
{
	if (ok != 0)
	{
		if (j > 0)
		{
			contor = -1;
			j -= 1;
		}

		glutPostRedisplay();
	}
}

void miscadr(void)
{
	if (ok != 0)
	{
		if (j < 400)
		{
			contor = 1;
			j += 1;
		}

		glutPostRedisplay();
	}
}

void keyboard(int key, int x, int y)
{

	switch (key) {
	case GLUT_KEY_LEFT:
		miscast();
		break;
	case GLUT_KEY_RIGHT:
		miscadr();
		break;

	}

}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Depaseste masinile - mini game");
	init();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard);

	glutMainLoop();
}