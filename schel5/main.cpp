#include <iostream>
#include<windows.h>
#include<string>
#include <utility>
#include<cmath>
#include <GL/freeglut.h>
#include <curl/curl.h>
#include <fstream>
#include <cstdio>
#include <vector>
#include <map>
#include <mmsystem.h>
#include <thread>

#pragma comment(lib, "winmm.lib")

#define PI 3.14159265359

// definesc pi/180 pentru calcule
#define K 0.0174532
#define M_PI 3.1415926


#define SPEED_INCREASE_PER_GRAPE 0.01 // cu cat cad mai repede strugurii cand se colecteaza unul
using namespace std;

typedef pair<float, float> point;

GLdouble left_m = -100.0;
GLdouble right_m = 700.0;
GLdouble bottom_m = -140.0;
GLdouble top_m = 460.0;
double ok = 1;
double j = 0.0;
double i = 0.0;
double contor = 0;
double loc_vert = 420; // loc_vert e de fapt loc_oriz, adica punctul pe orizontala unde pleaca toti strugurii
int vecpos[3] = { 0, 200, 400 };
double height = vecpos[rand() % 3]; // height va fi de fapt pozitia pe orizontala a cosului care prinde struguri
int score = 0;
double timp = 0.1, difModifier = 1;
int pct = 1000;
double rsj, rdj, rss, rds = 0;
int vieti = 3;
int currentProp = 0;
map<int, string, greater<int>> playerScores;
int window_width = 800, window_height = 600;
int currentMenuPhase = 0, currentInputSelect = -1, currentSceneIndex = 0, currentMenuHoverBtn = 0;
bool showErrorMsg = 0;
string username = "", password = "", errorMsg = "";
CURL* curl;
void (*currentScene)(void);

int l = 6;
int c = 181;
int l_curr = -1;

float scale_factor = 1.0f; // factorul de scalare
float pulse_speed = 0.01f; // viteza pulsului

typedef struct {
	float x, y;
}Point2D;

Point2D mat[6][181];

bool storeScoreRequest(int score);
void getLeaderboardDataRequest();

void resetStats() {
	contor = 0;
	loc_vert = 420; // loc_vert e de fapt loc_oriz, adica punctul pe orizontala unde pleaca toti strugurii
	height = vecpos[rand() % 3]; // height va fi de fapt pozitia pe orizontala a cosului care prinde struguri
	score = 0;
	difModifier = 1;
	vieti = 3;
	currentProp = 0;
	ok = 1;
}

void init(void)
{
	glClearColor(0.87, 0.8, 0.73, 0.0);
	glMatrixMode(GL_PROJECTION);
	glOrtho(left_m, right_m, bottom_m, top_m, -1.0, 1.0);
	gluOrtho2D(left_m - 1000, right_m + 1000, bottom_m - 1000, top_m + 1000);
}

void PlayMenuSong()
{
	PlaySound(L".\\music\\menu.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void PlayGameSong()
{
	PlaySound(L".\\music\\game.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void PlayEndSong()
{
	PlaySound(L".\\music\\end.wav", NULL, SND_FILENAME | SND_ASYNC);

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
	glDeleteLists(aux, 1); // Fixes mem leak
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
	glDeleteLists(aux, 1);
}

void RenderString(float x, float y, void* font, const unsigned char* string)
{

	glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2f(x, y);

	glutBitmapString(font, string);
}

void DrawRays(float cx, float cy, float r, float offset, int num_rays);
void DrawBezier(point m, point n, point p, int num_segments);

void menuBarrel()
{
	// Draw Barrel

	glColor3f(0.79, 0.56, 0.35);
	glBegin(GL_TRIANGLES);
	glVertex2f(185.8, -10.1);
	glVertex2f(174.6, -83.5);
	glVertex2f(420.2, -83.5);

	glVertex2f(420.2, -83.5);
	glVertex2f(403.6, -20.0);
	glVertex2f(185.8, -10.1);
	glEnd();

	glColor3f(0, 0, 0);
	DrawCircle(300, 180, 230, 30);
	glColor3f(0.79, 0.56, 0.35);
	DrawCircle(300, 180, 223, 30);

	glColor3f(0, 0, 0);
	DrawCircle(300, 180, 210, 30);
	glColor3f(0.79, 0.56, 0.35);
	DrawCircle(300, 180, 203, 30);


	glColor3f(0, 0, 0);
	glLineWidth(10.0f);
	DrawRays(300, 180, 205, 23, 15);

	// Draw barrel stand

	glLineWidth(5.0f);

	glBegin(GL_LINES);

	glVertex2f(185.8, -10.1);
	glVertex2f(177.1, -53.75);

	glVertex2f(175.1, -53.75);
	glVertex2f(187.0, -53.75);

	glVertex2f(174.6, -83.5);
	glVertex2f(420.2, -83.5);

	glVertex2f(420.2, -83.5);
	glVertex2f(403.6, -20.0);

	glEnd();
	DrawBezier(make_pair(185.9, -53.75), make_pair(170.8, -64.5), make_pair(174.6, -83.5), 3);
}

void startgame(void)
{
	if (vieti > 0) {
		if (i < -380)
		{
			i = 0;
		}
		i = i - 2 * timp;
		loc_vert -= timp * difModifier;


		//daca cosul e pe aceeasi coloana cu strugurele si in intervalul ..., atunci creste scorul
		if (height == j) {

			if (loc_vert > -100 && loc_vert < -50) {
				if (currentProp == 2)
					vieti--;
				else if (currentProp == 1) {
					score += 200;
				}
				else {
					score += 100;
				}
				height = vecpos[rand() % 3];
				currentProp = rand() % 3;
				difModifier = currentProp == 1 ? 2 : 1;
				loc_vert = 420;
				timp += SPEED_INCREASE_PER_GRAPE;			}
		}

		if (loc_vert <= -100) {
			if (currentProp != 2) {
				vieti--;
			}
			loc_vert = 420;
			height = vecpos[rand() % 3];
			currentProp = rand() % 3;
			difModifier = currentProp == 1 ? 2 : 1;
			timp += SPEED_INCREASE_PER_GRAPE;
		}
	}
	else {
		if (ok == 1) {
			ok = 0;
			storeScoreRequest(score);
			getLeaderboardDataRequest();
			PlayEndSong();
		}
	}
}

void deseneazaStrugure(float r, float g, float b) {

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
	//glColor3f(0.22, 0.07, 0.35); // mov inchis
	//glColor3f(0.51, 0.12, 0.63); // mov mai deschis decat cel de sus
	glColor3f(r, g, b);
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

void deseneazaInimi(float scale_factor) {

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

	glScalef(1, scale_factor, 1);
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
	glClearColor(0.87, 0.8, 0.73, 0.0);
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
	
	if (contor == 1 && (j != 200 && j != 400))
		j = j + 1;
	else if (contor == -1 && (j != 200 && j != 0))
		j = j - 1;
	else {
		contor = 0;
	}

	//desenam strugurele
	if (currentProp == 0) {
		glPushMatrix();
		glTranslatef(height, loc_vert, 0.0);
		deseneazaStrugure(0.51, 0.12, 0.63);
		glPopMatrix();
	}
	//desenam golden strugure
	else if (currentProp == 1) {
		glPushMatrix();
		glTranslatef(height, loc_vert, 0.0);
		deseneazaStrugure(1.0, 0.95, 0.0);
		glPopMatrix();
	}
	//desenam strugure bomba
	else if (currentProp == 2) {
		glPushMatrix();
		glTranslatef(height, loc_vert, 0.0);
		deseneazaStrugure(1.0, 0.0, 0.0);
		glPopMatrix();
	}

	// factor de scalare cu ajutorul functiei sinu
	scale_factor = 1.0f + sin(glutGet(GLUT_ELAPSED_TIME) * pulse_speed) * 0.01f;

	glPushMatrix();
		deseneazaInimi(scale_factor);
	glPopMatrix();
	
	glColor3f(0.78, 0.69, 0.65);
	glBegin(GL_POLYGON);
		glVertex2i(-100, 420);// Stanga jos
		glVertex2i(700, 420); // Dreapta jos
		glVertex2i(700, 460); // Dreapta sus
		glVertex2i(-100, 460);// Stanga sus
	glEnd();
	RenderString(200.0f, 432.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Prinde strugurii!");

	if (ok == 0) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0.0, 0.0, 0.0, 0.4);
		glRectf(-200, -200, 1000, 1000);

		menuBarrel();
		RenderString(220.0f, 300.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"GAME OVER");
		RenderString(205.0f, 270.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"LEADERBOARD:");

		// Render the leaderboard
		float x = 205.0f;
		float y = 250.0f;
		int rank = 1;
		for (const auto& player : playerScores) {
			if (y < 50) continue; // Only show 10
			string name = player.second;
			int score = player.first;
			string text = "Rank " + to_string(rank) + ": " + name + " - " + to_string(score);
			RenderString(x, y, GLUT_BITMAP_8_BY_13, (const unsigned char*)text.c_str());
			y -= 20.0f;
			rank++;
		}
		RenderString(225.0f, 20.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Back to menu");
		if (currentMenuHoverBtn == 1) {
			glLineWidth(2.0f);
			glBegin(GL_LINES);
			glVertex2f(225.0f, 10.0f);
			glVertex2f(365.0f, 10.0f);
			glEnd();
		}
	}

	startgame();
	glutPostRedisplay();
	glutSwapBuffers();
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	window_width = w; window_height = h;
	glutReshapeWindow(800, 600); // TODO: FIX THIS?
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100.0, 700.0, -140.0, 460.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void DrawRays(float cx, float cy, float r, float offset, int num_rays)
{
	// Set up circle parameters
	const float delta_theta = 2.0f * PI / num_rays;

	// Draw radii
	glBegin(GL_LINES);
	for (int i = 0; i < num_rays; ++i) {
		const float theta = delta_theta * i;
		const float x = cx + r * cos(theta);
		const float y = cy + r * sin(theta);
		const float ax = cx + (r + offset) * cos(theta);
		const float ay = cy + (r + offset) * sin(theta);
		//cout << i << " -- " << theta << endl;

	
	glVertex2f(x, y); // starting point
	glVertex2f(ax, ay); // endpoint
	}
	glEnd();
}

void DrawBezier(point m, point n, point p, int num_segments)
{
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= num_segments; i++) {
		float t = i / static_cast<float>(num_segments);
		float T = 1.0 - t;
		float x = T * T * m.first + 2.0 * T * t * n.first + t * t * p.first;
		float y = T * T * m.second + 2.0 * T * t * n.second + t * t * p.second;
		glVertex2f(x, y);
	}
	glEnd();
}

void DrawBottle(int i, int j)
{
	glColor3f(0.77, 0.18, 0.11);
	glRectf(i, j, i + 20, j+85);
	glBegin(GL_TRIANGLES);
	glVertex2f(i, j+85);
	glVertex2f(i + 20, j+85);
	glVertex2f(i + 4, j+105);

	glVertex2f(i + 10, j+85);
	glVertex2f(i + 4, j+105);
	glVertex2f(i + 16, j+105);

	glVertex2f(i + 10, j+85);
	glVertex2f(i + 16, j+105);
	glVertex2f(i + 20, j+85);
	glEnd();
	glRectf(i + 4, j+105, i + 16, j+108);

	glColor3f(0.58, 0.11, 0.09);
	glRectf(i + 4, j+108, i + 16, j+115);

	glColor3f(0.88, 0.55, 0.35);
	glRectf(i + 1.5, j+60, i + 18.5, j + 75);
}


void drawMenu(void)
{
	glClearColor(0.20, 0.09, 0.11, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.88, 0.48, 0.20);
	glRectf(-1000, -120, 1000, -130);
	glRectf(-1000, -100, 1000, -110);
	glRectf(-1000, -80, 1000, -90);
	glRectf(-1000, -60, 1000, -70);

	glColor3f(0.82, 0.42, 0.15);
	glRectf(-1000, -130, 1000, -140);
	glRectf(-1000, -110, 1000, -120);
	glRectf(-1000, -90, 1000, -100);
	glRectf(-1000, -70, 1000, -80);
	glRectf(-1000, -60, 1000, -55);

	

	glColor3f(0.88, 0.48, 0.20);
	glRectf(-100, 1000, -70, -55);
	glRectf(670, 1000, 700, -55);
	glRectf(670, 1000, 700, -55);
	glRectf(285, 1000, 315, -55);

	
	int i = -70, j = 315;
	
	for (int j = 315; j >= -100; j -= 185)
	{
		int i = -70;
		glColor3f(0.37, 0.21, 0.18);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(i, j);
		glVertex2f(i + 340, j);
		glVertex2f(i + 10, j + 10);
		glVertex2f(i + 330, j + 10);
		glEnd();

		glColor3f(0.29, 0.15, 0.15);

		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(i+360, j);
		glVertex2f(i + 350, j + 10);
		glVertex2f(i+360, j + 200);
		glVertex2f(i + 350, j + 200);
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(i, j);
		glVertex2f(i + 10, j + 10);
		glVertex2f(i, j + 200);
		glVertex2f(i + 10, j + 200);
		glEnd();

		for (int i = -40; i <= 270; i += 35) {
			DrawBottle(i, j+4);

		}
	}
	for (int j = 315; j >= -100; j -= 185)
	{
		int i = 330;
		glColor3f(0.37, 0.21, 0.18);
		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(i, j);
		glVertex2f(i + 340, j);
		glVertex2f(i + 10, j + 10);
		glVertex2f(i + 330, j + 10);
		glEnd();

		glColor3f(0.29, 0.15, 0.15);
		i = 310;
		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(i + 360, j);
		glVertex2f(i + 350, j + 10);
		glVertex2f(i + 360, j + 200);
		glVertex2f(i + 350, j + 200);
		glEnd();
		
		glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(i, j);
		glVertex2f(i + 10, j + 10);
		glVertex2f(i, j + 200);
		glVertex2f(i + 10, j + 200);
		glEnd();

		for (int i = 345; i <= 640; i += 35) {
			DrawBottle(i, j+4);

		}
	}

	glColor3f(0.71, 0.27, 0.12);
	glRectf(-70, 115, 670, 130);
	glRectf(-70, 300, 670, 315);

	menuBarrel();

	RenderString(180.0f, 280.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"<NUMELE JOCULUI>");
	
	if (currentMenuPhase == 0 || currentMenuPhase == 1) {
		// Draw login form

		// Username input
		if (currentInputSelect == 0)
			glColor3f(0, 1, 0);
		else
			glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex2f(200, 200);
		glVertex2f(400, 200);
		glEnd();
		if (username == "")
			RenderString(200.0f, 210.0f, GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Username...");
		else
			RenderString(200.0f, 210.0f, GLUT_BITMAP_HELVETICA_18, (const unsigned char*)username.c_str());

		// Password input
		if (currentInputSelect == 1)
			glColor3f(0, 1, 0);
		else
			glColor3f(0, 0, 0);
		glBegin(GL_LINES);
		glVertex2f(200, 140);
		glVertex2f(400, 140);
		glEnd();
		if (password == "")
			RenderString(200.0f, 150.0f, GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Password...");
		else {
			string asterisks(password.size(), '*');
			RenderString(200.0f, 150.0f, GLUT_BITMAP_HELVETICA_18, (const unsigned char*)asterisks.c_str());
		}
	}
	if (currentMenuPhase == 0) {
		// Signup btn
		RenderString(250.0f, 90.0f, GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Signup here");
		if (currentMenuHoverBtn == 1)
		{
			glLineWidth(2.0f);
			glBegin(GL_LINES);
			glVertex2f(250, 85);
			glVertex2f(350, 85);
			glEnd();
		}

		// Login btn

		RenderString(270.0f, 50.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Login");
		if (currentMenuHoverBtn == 2)
		{
			glLineWidth(2.0f);
			glBegin(GL_LINES);
			glVertex2f(270, 45);
			glVertex2f(330, 45);
			glEnd();
		}
	}
	if (currentMenuPhase == 1) {
		// Back btn
		RenderString(275.0f, 90.0f, GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Back");
		if (currentMenuHoverBtn == 1)
		{
			glLineWidth(2.0f);
			glBegin(GL_LINES);
			glVertex2f(275, 85);
			glVertex2f(320, 85);
			glEnd();
		}

		// Signup btn
		RenderString(260.0f, 50.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Sign up");
		if (currentMenuHoverBtn == 2)
		{
			glLineWidth(2.0f);
			glBegin(GL_LINES);
			glVertex2f(260, 45);
			glVertex2f(340, 45);
			glEnd();
		}
	}
	if (currentMenuPhase == 2) {
		glColor3f(0, 0, 0);
		glLineWidth(2.0f);
		RenderString(180.0f, 250.0f, GLUT_BITMAP_HELVETICA_18, (const unsigned char*)("Hello, " + username + "!").c_str());

		RenderString(240.0f, 200.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*) "Start game");
		if (currentMenuHoverBtn == 1) {
			glBegin(GL_LINES);
			glVertex2f(240, 190);
			glVertex2f(345, 190);
			glEnd();
		}
		RenderString(250.0f, 150.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*) "Sign out");
		if (currentMenuHoverBtn == 2) {
			glBegin(GL_LINES);
			glVertex2f(250, 140);
			glVertex2f(330, 140);
			glEnd();
		}
		RenderString(270.0f, 100.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*) "Exit");
		if (currentMenuHoverBtn == 3) {
			glBegin(GL_LINES);
			glVertex2f(270, 90);
			glVertex2f(310, 90);
			glEnd();
		}
	}
	if (showErrorMsg) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0.0, 0.0, 0.0, 0.4); 
		glRectf(-200, -200, 1000, 1000);

		glColor3f(0, 0, 0);
		glRectf(60, 70, 540, 290);

		glColor3f(0.79, 0.56, 0.35);
		glRectf(70, 80, 530, 280);

		glColor3f(0, 0, 0);
		RenderString(100.0f, 230.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)("Error: " + errorMsg).c_str());
		RenderString(271.0f, 100.0f, GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"Back");
	}
	glutPostRedisplay();
	glutSwapBuffers();
	glFlush();
}

string extractMessage(const string& jsonString) {
	string message;

	size_t messageStart = jsonString.find("\"message\"");

	if (messageStart != string::npos) {
		size_t valueStart = jsonString.find("\"", messageStart + 9); 
		size_t valueEnd = jsonString.find("\"", valueStart + 1);

		if (valueStart != string::npos && valueEnd != string::npos) {
			message = jsonString.substr(valueStart + 1, valueEnd - valueStart - 1);
		}
	}

	return message;
}

vector<pair<int, string>> parseScores(const string& input) {
	vector<pair<int, string>> result;

	size_t start = input.find('[');
	if (start == string::npos) {
		cerr << "Invalid input format: missing starting '['" << endl;
		return result;
	}

	size_t end = input.find(']', start);
	if (end == string::npos) {
		cerr << "Invalid input format: missing ending ']'" << endl;
		return result;
	}

	// Extract the array substring
	string arrayStr = input.substr(start + 1, end - start - 1);

	// Parse each object within the array
	size_t pos = 0;
	while (pos < arrayStr.length()) {
		// Find the starting index of the object
		size_t objStart = arrayStr.find('{', pos);
		if (objStart == string::npos) {
			break; // No more objects found
		}

		// Find the ending index of the object
		size_t objEnd = arrayStr.find('}', objStart);
		if (objEnd == string::npos) {
			cerr << "Invalid input format: missing ending '}'" << endl;
			return result;
		}

		// Extract the object substring
		string objStr = arrayStr.substr(objStart + 1, objEnd - objStart - 1);

		// Parse the score and username within the object
		size_t scoreStart = objStr.find("\"score\":") + 8;
		size_t scoreEnd = objStr.find(',', scoreStart);
		if (scoreEnd == string::npos) {
			cerr << "Invalid input format: missing ','" << endl;
			return result;
		}
		int score = stoi(objStr.substr(scoreStart, scoreEnd - scoreStart));

		size_t usernameStart = objStr.find("\"username\":\"") + 12;
		size_t usernameEnd = objStr.find('\"', usernameStart);
		if (usernameEnd == string::npos) {
			cerr << "Invalid input format: missing '\"'" << endl;
			return result;
		}
		string username = objStr.substr(usernameStart, usernameEnd - usernameStart);

		// Add the score and username to the result vector
		result.emplace_back(score, username);

		// Move the position to the next object
		pos = objEnd + 1;
	}

	return result;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* response) {
	size_t totalSize = size * nmemb;
	response->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

bool loginRequest()
{
	CURL* curl = curl_easy_init();
	bool ok = false;
	if (curl) {
		string postData = "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";

		curl_easy_setopt(curl, CURLOPT_URL, "https://grafica-backend.onrender.com/login");
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		string response;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			cerr << "LOGIN FAILED: " << curl_easy_strerror(res) << endl;
			ok = false;
			errorMsg = extractMessage(response);
		}
		else {
			long responseCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
			if (responseCode != 200) {
				errorMsg = extractMessage(response);
				cerr << "LOGIN FAILED: " << errorMsg << endl;
				ok = false;
			}
			else {
				cout << "LOGIN RESP: " << response << endl;
				ok = true;
			}
		}
		
		curl_global_cleanup();
	}
	return ok;

}

bool signupRequest()
{
	CURL* curl = curl_easy_init();
	bool ok = true;
	if (curl) {
		string postData = "{\"username\":\"" + username + "\",\"password\":\"" + password + "\"}";
		
		curl_easy_setopt(curl, CURLOPT_URL, "https://grafica-backend.onrender.com/signup");
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		string response;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			cerr << "SIGNUP FAILED: " << curl_easy_strerror(res) << endl;
			ok = false;
			errorMsg = extractMessage(response);
		}
		else {
			long responseCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
			if (responseCode != 200) {
				errorMsg = extractMessage(response);
				cerr << "SIGNUP FAILED: " << errorMsg << endl;
				ok = false;
			}
			else {
				cout << "SIGNUP RESP: " << response << endl;
				ok = true;
			}
		}
		curl_global_cleanup();
	}
	return ok;
}

bool storeScoreRequest(int score)
{
	CURL* curl = curl_easy_init();
	bool ok = true;
	if (curl) {
		string postData = "{\"username\":\"" + username + "\",\"score\":\"" + to_string(score) + "\"}";

		curl_easy_setopt(curl, CURLOPT_URL, "https://grafica-backend.onrender.com/store_score");
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());

		struct curl_slist* headers = nullptr;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		string response;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		CURLcode res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			cerr << "STORE SCORE FAILED: " << curl_easy_strerror(res) << endl;
			ok = false;
			errorMsg = extractMessage(response);
		}
		else {
			long responseCode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
			if (responseCode != 200) {
				errorMsg = extractMessage(response);
				cerr << "STORE SCORE FAILED: " << errorMsg << endl;
				ok = false;
			}
			else {
				cout << "STORE SCORE RESP: " << response << endl;
				ok = true;
			}
		}
		curl_global_cleanup();
	}
	return ok;
}

void getLeaderboardDataRequest()
{
	CURL* curl = curl_easy_init();
	vector<pair<int, string>> scores;
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://grafica-backend.onrender.com/top_scores");

		string response;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		CURLcode res = curl_easy_perform(curl);
		cout << response << endl;
		scores = parseScores(response);
		for (const auto& score : scores) {
			playerScores[score.first] = score.second;
		}
	}
}

void saveUser()
{
	ofstream outputFile("saved.txt");
	if (outputFile.is_open()) {
		outputFile << username << endl;
		outputFile << password << endl;

		outputFile.close();

		cout << "User saved successfully." << endl;
	}
	else {
		cerr << "Failed to open user save file." << endl;
	}
}

bool rememberUser()
{
	ifstream inputFile("saved.txt");
	if (inputFile.is_open()) {
		getline(inputFile, username);
		getline(inputFile, password);

		inputFile.close();
		if (!loginRequest()) {
			username = "";
			password = "";
			cerr << "Invalid user in save file" << endl;
			return 0;
		}
		else {
			cout << "User loaded successfully." << endl;
			return 1;
		}
	}
	else {
		cerr << "Failed to open user save file." << endl;
		return 0;
	}
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
	if (currentSceneIndex == 1) {
		switch (key) {
		case GLUT_KEY_LEFT:
			miscast();
			break;
		case GLUT_KEY_RIGHT:
			miscadr();
			break;

		}
	}

}

void handleClick(int button, int state, int x, int y)
{
	if (showErrorMsg == 1) {
		if (state == GLUT_DOWN) {
			if (700 - x >= 270 && 700 - x <= 330 && 460 - y >= 90 && 460 - y <= 120)
			{
				showErrorMsg = 0;
			}
		}
	}
	else if (currentSceneIndex == 0) {
		if (currentMenuPhase == 0 || currentMenuPhase == 1) {
			if (state == GLUT_DOWN) {
				if (x >= 200 && x <= 500 && 460 - y >= 200 && 460 - y <= 240)
					currentInputSelect = 0;
				if (x >= 200 && x <= 500 && 460 - y >= 140 && 460 - y <= 180)
					currentInputSelect = 1;

				if (700 - x >= 250 && 700 - x <= 350 && 460 - y >= 85 && 460 - y <= 110) {
					username = "";
					password = "";
					currentInputSelect = -1;
					currentMenuPhase = currentMenuPhase == 0 ? 1 : 0;
				}
				if (700 - x >= 270 && 700 - x <= 330 && 460 - y >= 45 && 460 - y <= 70)
				{
					if (currentMenuPhase == 0) {
						if (loginRequest()) {
							saveUser();
							currentMenuPhase = 2;
						}
						else {
							showErrorMsg = 1;
						}
					}
					else {
						if (signupRequest()) {
							saveUser();
							currentMenuPhase = 2;
						}
						else {
							showErrorMsg = 1;
						}
					}
				}
			}
		}
		else if (currentMenuPhase == 2) {
			if (state == GLUT_DOWN) {
				x = 700 - x;
				if (x >= 240 && x <= 350 && 460 - y >= 190 && 460 - y <= 230)
				{
					currentSceneIndex = 1;
					resetStats();
					PlayGameSong();
					glutDisplayFunc(drawScene);
					currentMenuHoverBtn = 0;
				}
				else if (x >= 250 && x <= 340 && 460 - y >= 140 && 460 - y <= 180)
				{
					currentMenuPhase = 0;
					username = "";
					password = "";
					remove("saved.txt");
				}
				else if (x >= 270 && x <= 320 && 460 - y >= 90 && 460 - y <= 130)
				{
					exit(0);
				}
				else currentMenuHoverBtn = 0;
			}
		}
	}
	else {
		if (ok == 0 && state == GLUT_DOWN) {
			x = 700 - x;
			if (x >= 225 && x <= 370 && 460 - y >= 0 && 460 - y <= 50)
			{
				currentSceneIndex = 0;
				PlayMenuSong();
				glutDisplayFunc(drawMenu);
				currentMenuHoverBtn = 0;
			}
		}
	}
}

void handleHover(int x, int y)
{
	if (currentSceneIndex == 0) {
		if (currentMenuPhase == 0 || currentMenuPhase == 1) {
			x = 700 - x;
			if (x >= 250 && x <= 350 && 460 - y >= 85 && 460 - y <= 110)
				currentMenuHoverBtn = 1;
			else if (x >= 270 && x <= 330 && 460 - y >= 45 && 460 - y <= 70)
				currentMenuHoverBtn = 2;
			else currentMenuHoverBtn = 0;
		}
		if (currentMenuPhase == 2) {
			x = 700 - x;
			if (x >= 240 && x <= 350 && 460 - y >= 190 && 460 - y <= 230)
				currentMenuHoverBtn = 1;
			else if (x >= 250 && x <= 340 && 460 - y >= 140 && 460 - y <= 180)
				currentMenuHoverBtn = 2;
			else if (x >= 270 && x <= 320 && 460 - y >= 90 && 460 - y <= 130)
				currentMenuHoverBtn = 3;
			else currentMenuHoverBtn = 0;
		}
	}
	else {
		if (ok == 0) {
			x = 700 - x;
			if (x >= 225 && x <= 370 && 460 - y >= 0 && 460 - y <= 50)
			{
				currentMenuHoverBtn = 1;
			}
			else currentMenuHoverBtn = 0;
		}
	}
}

void handleInput(unsigned char key, int x, int y)
{
	if (currentSceneIndex == 0) {
		if (currentMenuPhase == 0 || currentMenuPhase == 1) {
			if ((int)key == 9) {
				currentInputSelect = (currentInputSelect + 1) % 2;
			}
			else {
				if (currentInputSelect == 0) {
					if (key == 8 && username.size() > 0) {
						username.pop_back();
					}
					else {
						username.push_back(key);
					}
				}
				else if (currentInputSelect == 1) {
					if (key == 8 && password.size() > 0) {
						password.pop_back();
					}
					else {
						password.push_back(key);
					}
				}
			}
		}
	}
}


int main(int argc, char** argv)
{
	currentScene = &drawMenu;
	curl_global_init(CURL_GLOBAL_ALL);
	if (rememberUser())
		currentMenuPhase = 2;
	PlayMenuSong();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Prinde strugurii");
	init();
	glutDisplayFunc(drawMenu);
	glutMouseFunc(handleClick);
	glutPassiveMotionFunc(handleHover);
	glutReshapeFunc(reshape);
	glutSpecialFunc(keyboard);
	glutKeyboardFunc(handleInput);
	glutMainLoop();
}
