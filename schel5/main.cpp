#include <iostream>
#include<windows.h>
#include<string>
#include<cmath>
#include <GL/freeglut.h>

// definesc pi/180 pentru calcule
#define K 0.0174532
#define M_PI 3.1415926

using namespace std;

GLdouble left_m = -100.0;
GLdouble right_m = 700.0;
GLdouble bottom_m = -140.0;
GLdouble top_m = 460.0;
double ok = 1;
double j = 0.0;
double i = 0.0;
double contor = 0;
double loc_vert = 420; // loc_vert e de fapt loc_oriz, adica punctul pe orizontala unde pleaca toti strugurii
int vector[3] = { 0, 200, 400 };
double height = vector[rand() % 3]; // height va fi de fapt pozitia pe orizontala a cosului care prinde struguri
int score = 0;
double timp = 0.1;
int pct = 1000;
double rsj, rdj, rss, rds = 0;
int vieti = 3;

int l = 6;
int c = 181;
int l_curr = -1;

typedef struct {
	float x, y;
}Point2D;

Point2D mat[6][181];

void init(void)
{
	glClearColor(0.87, 0.8, 0.73, 0.0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(left_m, right_m, bottom_m, top_m, -1.0, 1.0);
	gluOrtho2D(left_m - 1000, right_m + 1000, bottom_m - 1000, top_m + 1000);
}


void genereaza_pct_arc_elipsa(float a, float b, float start_angle, float end_angle) {
	int nr = 0;
	l_curr++;
	l_curr = l_curr % l;
	float theta = start_angle * K;
	float dtheta = K; // o unitate dintr-un radian
	float x = a * cosf(theta);
	float y = b * sinf(theta);
	
	while (theta <= end_angle * K) {
		mat[l_curr][nr].x = x;
		mat[l_curr][nr].y = y;
		if (nr < c) {
			nr++;
		}
		else {
			break;
		}
		if (theta >= 360) {
			theta = 0;
		}
		theta += dtheta;
		x = a * cosf(theta);
		y = b * sinf(theta);
	}
}

int vector_puncte(Point2D*& v, int linie, float sup_x, float inf_x, float val, bool ok) {
	int nr = 0;
	if (ok == true) {
		for (int i = 0; i < c; i++)
			if (abs(mat[linie][i].x) <= abs(val - inf_x) && abs(mat[linie][i].x) >= abs(val - sup_x)) {
				nr++;
			}

		if (nr > 0) {
			v = new Point2D[nr];
			int j = -1;
			for (int i = 0; i < c; i++)
				if (abs(mat[linie][i].x) <= abs(val - inf_x) && abs(mat[linie][i].x) >= abs(val - sup_x)) {
					v[++j] = mat[linie][i];
				}
		}
	}
	else {
		for (int i = 0; i < c; i++)
			if (abs(mat[linie][i].x) >= abs(val - inf_x) && abs(mat[linie][i].x) <= abs(val - sup_x)) {
				nr++;
			}

		if (nr > 0) {
			v = new Point2D[nr];
			int j = -1;
			for (int i = 0; i < c; i++)
				if (abs(mat[linie][i].x) >= abs(val - inf_x) && abs(mat[linie][i].x) <= abs(val - sup_x)) {
					v[++j] = mat[linie][i];
				}
		}
	}
	return nr;
}


//void DrawCircle(float cx, float cy, float r, int num_segments)
//{
//	GLuint aux;
//	aux = glGenLists(1);
//	glNewList(aux, GL_COMPILE);
//	glBegin(GL_POLYGON);
//	for (int ii = 0; ii < num_segments; ii++)
//	{
//		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle
//
//		float x = r * cosf(theta);//calculate the x component
//		float y = r * sinf(theta);//calculate the y component
//
//		glVertex2f(x + cx, y + cy);//output vertex
//
//	}
//	glEnd();
//	glEndList();
//	glCallList(aux);
//}

void DrawElipse(float cx, float cy, float r1, float r2, int num_segments)
{
	GLuint aux;
	aux = glGenLists(1);
	glNewList(aux, GL_COMPILE);
	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

		float x = r1 * cosf(theta);//calculate the x component
		float y = r2 * sinf(theta);//calculate the y component

		glVertex2f(x + cx, y + cy);//output vertex

	}
	glEnd();
	glEndList();
	glCallList(aux);
}

void RenderString(float x, float y, void* font, const unsigned char* string)
{

	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2f(x, y);

	glutBitmapString(font, string);
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
				loc_vert = 420;
			}
		}

		if (loc_vert <= -100) {
			vieti--;
			loc_vert = 420;
			height = vector[rand() % 3];
		}

	}
	else {
		ok = 0;
	}
}

void deseneazaStrugure() {

	// ramura
	glColor3f(0.56, 0.34, 0.21);
	glRecti(-3, 35, 3, 15);
	//DrawCircle(0, 35, 3, 10);

	glEnable(GL_POINT_SMOOTH);
	glPointSize(6.0);
	glBegin(GL_POINTS);
	glVertex2f(0, 35);
	glEnd();

	// boabele
	glColor3f(0.22, 0.07, 0.35); // mov inchis
	glColor3f(0.51, 0.12, 0.63); // mov mai deschis decat cel de sus

	/*DrawCircle(-15, 15, 7, 100);
	DrawCircle(-5, 15, 7, 100);
	DrawCircle(5, 15, 7, 100);
	DrawCircle(15, 15, 7, 100);

	DrawCircle(-10, 5, 7, 100);
	DrawCircle(0, 5, 7, 100);
	DrawCircle(10, 5, 7, 100);

	DrawCircle(-5, -5, 7, 100);
	DrawCircle(5, -5, 7, 100);

	DrawCircle(0, -15, 7, 100);*/

	glEnable(GL_POINT_SMOOTH);
	glPointSize(12.0);
	glBegin(GL_POINTS);
	glVertex2f(-15, 15);
	glVertex2f(-5, 15);
	glVertex2f(5, 15);
	glVertex2f(15, 15);

	glVertex2f(-10, 5);
	glVertex2f(0, 5);
	glVertex2f(10, 5);

	glVertex2f(-5, -5);
	glVertex2f(5, -5);

	glVertex2f(0, -15);
	glEnd();

	// frunza
	glColor3f(0.2, 0.67, 0.13);
	DrawElipse(12, 21, 12, 5, 100);
}

void deseneazaInimi() {

	string sir = "Scor: " + to_string(score);

	const char* str = sir.c_str();

	RenderString(530.0f, 400.0f, GLUT_BITMAP_9_BY_15, (const unsigned char*)str);

	glColor3f(0.78, 0.16, 0.15);

	int x_st = 520, y_st = 350, dist = 50;

	/*for (int i = 0; i < vieti; i++)
	{
		DrawCircle(-10 + x_st + i * dist, 0 + y_st, 10, 50);
		DrawCircle(10 + x_st + i * dist, 0 + y_st, 10, 50);

		glBegin(GL_TRIANGLES);
		glVertex2f(-20 + x_st + i * dist, -3 + y_st);
		glVertex2f(20 + x_st + i * dist, -3 + y_st);
		glVertex2f(0 + x_st + i * dist, -30 + y_st);
		glEnd();
	}*/


	for (int i = 0; i < vieti; i++)
	{
		glEnable(GL_POINT_SMOOTH);
		glPointSize(20.0);
		glBegin(GL_POINTS);
			glVertex2f(-10 + x_st + i * dist, 0 + y_st);
			glVertex2f(10 + x_st + i * dist, 0 + y_st);
		glEnd();

		glBegin(GL_TRIANGLES);
			glVertex2f(-20 + x_st + i * dist, -3 + y_st);
			glVertex2f(20 + x_st + i * dist, -3 + y_st);
			glVertex2f(0 + x_st + i * dist, -30 + y_st);
		glEnd();
	}
}

void deseneazaLada() {

	glColor3f(0.71, 0.28, 0.0);
	glRecti(-45, -15, -35, 30);
	glRecti(35, -15, 45, 30);

	glColor3f(0.8, 0.48, 0.13);
	glRecti(-45, -15, 45, 0);
	glRecti(-45, 5, 45, 20);

}

void deseneaza_butoi() {

	glColor3f(0.552, 0.321, 0.121);
	glLineWidth(2.0);

// generare varfuri pentru fiecare arc ce compune butoiul
// ---------------------------------------------------------
	//1. arc elipsa stanga
	genereaza_pct_arc_elipsa(20, 95, 90, 270);
	//2. arc elipsa dreapta
	genereaza_pct_arc_elipsa(20, 95, 270, 450);
	//3. interior 1
	genereaza_pct_arc_elipsa(16, 95, 90, 270);
	//4. interior 2
	genereaza_pct_arc_elipsa(8, 95, 90, 270);
	//5. interior 3
	genereaza_pct_arc_elipsa(8, 95, 270, 450);
	//6. interior 4
	genereaza_pct_arc_elipsa(16, 95, 270, 450);
// ---------------------------------------------------------
	
// forma butoi
// ---------------------------------------------------------
	// linie jos + arc stanga
	glPushMatrix();
	glTranslatef(490, 10, 0.0);
	glBegin(GL_POLYGON);
		// linie jos
		glVertex2f(0, -95);
		glVertex2f(180, -95);

		for (int i = 0; i < c; i++) {
			glVertex2f(mat[0][i].x, mat[0][i].y);
		}
	glEnd();
	glPopMatrix();

	// linie sus + arc dreapta
	glPushMatrix();
	glTranslatef(670, 10, 0.0);
	glBegin(GL_POLYGON);
		// linie sus
		glVertex2f(0, 95);
		glVertex2f(-180, 95);

		for (int i = 0; i < c; i++) {
			glVertex2f(mat[1][i].x, mat[1][i].y);
		}
	glEnd();
	glPopMatrix();

// contur butoi exterior
// ---------------------------------------------------------
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex2f(490, -85); // 180 dif de 36
		glVertex2f(670, -85);

		glVertex2f(490, 105);
		glVertex2f(670, 105);
	glEnd();

	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < c; i++) {
			glVertex2f(490 + mat[0][i].x, 10 + mat[0][i].y);
		}
	glEnd();

	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < c; i++) {
			glVertex2f(670 + mat[1][i].x, 10 + mat[1][i].y);
		}
	glEnd();
// ---------------------------------------------------------

// culori intermediare
// ---------------------------------------------------------
	glColor3f(0.682, 0.427, 0.203);
	glBegin(GL_POLYGON);
	for (int i = 0; i < c; i++) {
		glVertex2f(526 + mat[2][i].x, 10 + mat[2][i].y);
		glVertex2f(562 + mat[3][i].x, 10 + mat[3][i].y);
	}
	glEnd();

	glBegin(GL_POLYGON);
	for (int i = 0; i < c; i++) {
		glVertex2f(634 + mat[5][i].x, 10 + mat[5][i].y);
		glVertex2f(598 + mat[4][i].x, 10 + mat[4][i].y);
	}
	glEnd();
// ---------------------------------------------------------
	

// arce interioare butoi
// ---------------------------------------------------------
	glColor3f(0.0, 0.0, 0.0);
// interior 1
	glPushMatrix();
	glTranslatef(526, 10, 0.0);
	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < c; i++) {
			glVertex2f(mat[2][i].x, mat[2][i].y);
		}
	glEnd();
	glPopMatrix();

// interior 2
	glPushMatrix();
	glTranslatef(562, 10, 0.0);
	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < c; i++) {
			glVertex2f(mat[3][i].x, mat[3][i].y);
		}
	glEnd();
	glPopMatrix();

// interior 3
	glPushMatrix();
	glTranslatef(598, 10, 0.0);
	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < c; i++) {
			glVertex2f(mat[4][i].x, mat[4][i].y);
		}
	glEnd();
	glPopMatrix();

// interior 4
	glPushMatrix();
	glTranslatef(634, 10, 0.0);
	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < c; i++) {
			glVertex2f(mat[5][i].x, mat[5][i].y);
		}
		glEnd();
	glPopMatrix();
// ---------------------------------------------------------


// bare butoi
// ---------------------------------------------------------
	Point2D* v, * p;
	int nr = 0;
	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
		nr = vector_puncte(v, 0, 475.0, 472.0, 490.0, 1);
		for (int i = 0; i < nr; i++)
			if (v[i].y >= 0)
				glVertex2f(490 + v[i].x, 10 + v[i].y);
		
		nr = vector_puncte(p, 1, 688.0, 685.0, 670.0, 0);
		for (int i = 0; i < nr; i++)
			if (p[i].y >= 0)
				glVertex2f(670 + p[i].x, 10 + p[i].y);
		delete[] v;
		delete[] p;
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex2f(475.0, 71.0);
		glVertex2f(685.0, 71.0);

		glVertex2f(472.0, 52.0);
		glVertex2f(688.0, 52.0);
	glEnd();

	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
		nr = vector_puncte(v, 0, 475.0, 472.0, 490.0, 1);
		for (int i = 0; i < nr; i++)
			if (v[i].y <= 0)
				glVertex2f(490 + v[i].x, 10 + v[i].y);

		nr = vector_puncte(p, 1, 688.0, 685.0, 670.0, 0);
		for (int i = 0; i < nr; i++)
			if (p[i].y <= 0)
				glVertex2f(670 + p[i].x, 10 + p[i].y);
		delete[] v;
		delete[] p;
	glEnd();
	

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex2f(475.0, -51.0);
		glVertex2f(685.0, -51.0);

		glVertex2f(472.0, -31.0);
		glVertex2f(688.0, -31.0);
	glEnd();

// "suruburi" butoi
// ---------------------------------------------------------
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(11.0);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
		glVertex2f(491, 62);
		glVertex2f(532, 62);
		glVertex2f(578, 62);
		glVertex2f(625, 62);
		glVertex2f(667, 62);

		glVertex2f(491, -41);
		glVertex2f(532, -41);
		glVertex2f(578, -41);
		glVertex2f(625, -41);
		glVertex2f(667, -41);
	glEnd();


	glColor3f(1.0, 1.0, 1.0);
	glPointSize(7.0);
	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
		glVertex2f(491, 62);
		glVertex2f(532, 62);
		glVertex2f(578, 62);
		glVertex2f(625, 62);
		glVertex2f(667, 62);

		glVertex2f(491, -41);
		glVertex2f(532, -41);
		glVertex2f(578, -41);
		glVertex2f(625, -41);
		glVertex2f(667, -41);
	glEnd();
// ---------------------------------------------------------
}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.78, 0.69, 0.65);
	glBegin(GL_POLYGON);
		glVertex2i(460, -140);// Stanga jos
		glVertex2i(700, -140); // Dreapta jos
		glVertex2i(700, 460);  // Dreapta sus
		glVertex2i(460, 460); // Stanga sus
	glEnd();

	deseneaza_butoi();

	//desenam cosul/butoiul/lada
	glPushMatrix();
		glTranslatef(j, -100.0, 0.0);
		deseneazaLada();
		if (ok == 0)
		{
			rsj = 8;
			rss = -8;
			rdj = -8;
			rds = 8;
		}
	glPopMatrix();

	if (ok == 0) {
		RenderString(250.0f, 200.0f, GLUT_BITMAP_8_BY_13, (const unsigned char*)"GAME OVER");
	}

	if (contor == 1 && (j != 200 && j != 400)) {
		j = j + 1;
	}
	else if (contor == -1 && (j != 200 && j != 0)) {
		j = j - 1;
	}
	else {
		contor = 0;
	}

	//desenam strugurele
	glPushMatrix();
		glTranslatef(height, loc_vert, 0.0);
		deseneazaStrugure();
	glPopMatrix();

	deseneazaInimi();

	glColor3f(0.78, 0.69, 0.65);
	glBegin(GL_POLYGON);
		glVertex2i(-100, 420);// Stanga jos
		glVertex2i(700, 420); // Dreapta jos
		glVertex2i(700, 460); // Dreapta sus
		glVertex2i(-100, 460);// Stanga sus
	glEnd();
	RenderString(200.0f, 432.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Prinde strugurii!");

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
	glOrtho(-100.0, 700.0, -140.0, 460.0, -1.0, 1.0);
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
	glutCreateWindow("Prinde strugurii");
	init();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard);

	glutMainLoop();
}
