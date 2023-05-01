#include <iostream>
#include <windows.h>
#include <string>
#include <GL/freeglut.h>
#include <SOIL.h>
#include <map>
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
int veclanes[3] = { 0, 200, 400 };
double height = veclanes[rand() % 3]; // height va fi de fapt pozitia pe orizontala a cosului care prinde struguri
int score = 0;
double timp = 0.1;
int pct = 1000;
double rsj, rdj, rss, rds = 0;
int vieti = 3;
map<int, string, greater<int>> playerScores;

void init(void)
{
	glClearColor(0.87, 0.8, 0.73, 0.0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(left_m, right_m, bottom_m, top_m, -1.0, 1.0);
}

void DrawCircle(float cx, float cy, float r, int num_segments)
{
	GLuint aux;
	aux = glGenLists(1);
	glNewList(aux, GL_COMPILE);
	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++)
	{
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex2f(x + cx, y + cy);//output vertex

	}
	glEnd();
	glEndList();
	glCallList(aux);
}

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
				height = veclanes[rand() % 3];
				loc_vert = 420;
			}
		}

		if (loc_vert <= -100) {
			vieti--;
			loc_vert = 420;
			height = veclanes[rand() % 3];
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


	//desenam cosul/butoiul/lada
	glPushMatrix();
	glTranslated(j, -100.0, 0.0);

	//glColor3f(0.996, 0.365, 0.149);
	deseneazaLada();

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
		// Update a player's score
		string playerName = "Player 1";
		int newScore = score;
		for (auto& player : playerScores) {
			if (player.second == playerName) {
				playerScores.erase(player.first); // Remove old score
				playerScores[newScore] = playerName; // Add new score
				break; // Stop searching once found
			}
		}

		RenderString(530.0f, 380.0f, GLUT_BITMAP_8_BY_13, (const unsigned char*)"LEADERBOARD:");

		// Render the leaderboard
		float x = 490.0f;
		float y = 360.0f;
		int rank = 1;
		for (const auto& player : playerScores) {
			string name = player.second;
			int score = player.first;
			string text = "Rank " + to_string(rank) + ": " + name + " - " + to_string(score);
			RenderString(x, y, GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
			y -= 20.0f;
			rank++;
		}
	}

	if (contor == 1 && (j != 200 && j != 400))
		j = j + 1;
	else if (contor == -1 && (j != 200 && j != 0))
		j = j - 1;
	else {
		contor = 0;
	}

	//desenam strugurele
	glPushMatrix();
	glTranslated(height, loc_vert, 0.0);
	deseneazaStrugure();
	glPopMatrix();

	deseneazaInimi();
	glColor3f(0.78, 0.69, 0.65);
	// Iarba de sus
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
	playerScores[100] = "Player 1";
	playerScores[900] = "Player 2";
	playerScores[80] = "Player 3";
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