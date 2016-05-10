# include <iostream>
# include <gsim/gs.h>
# include <stdlib.h>
# include "app_window.h"

using namespace std;
int score = 0;

AppWindow::AppWindow(const char* label, int x, int y, int w, int h)
	:GlutWindow(label, x, y, w, h)
{
	initPrograms();
	addMenuEntry("Option 0", evOption0);
	addMenuEntry("Option 1", evOption1);
	_viewaxis = true;
	_up = _down = _right = _left = _gameover = _direction_change = start = hit_fruit = false;
	_fovy = GS_TORAD(60.0f);
	_rotx = _roty = 0;
	_w = w;
	_h = h;
	up = 0.0;
	right = 0.0;
	size = 2;
	test = 0.0f;

	randx = -0.5f;
	randy = 0.5f;

	xx.push_back(0.0f);
	yy.push_back(0.0f);
	xx.push_back(0.05f);
	yy.push_back(0.0f);

}

void AppWindow::initPrograms()
{
	// Load your shaders and link your programs here:
	_vertexsh.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_flat.glsl");
	_fragsh.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_flat.glsl");
	_prog.init_and_link(_vertexsh, _fragsh);

	// Init my scene objects:
	_axis.init(_prog);
	_fruit.init(_prog);
	_snake.init(_prog);
}

// mouse events are in window coordinates, but your 2D scene is in [0,1]x[0,1],
// so make here the conversion when needed
GsVec2 AppWindow::windowToScene(const GsVec2& v)
{
	// GsVec2 is a lighteweight class suitable to return by value:
	return GsVec2((2.0f*(v.x / float(_w))) - 1.0f,
		1.0f - (2.0f*(v.y / float(_h))));
}

// Called every time there is a window event
void AppWindow::glutKeyboard(unsigned char key, int x, int y)
{
	if (start == false) {
		switch (key)
		{

		case 'w': if (_down == false) { _down = _right = _left = false;  _up = true; redraw(); } start = true; break;
		case 's': if (_up == false) { _up = _right = _left = false; _down = true; redraw(); }start = true; break;
		case 'a': if (_right == false) { _down = _right = _up = false; _left = true; redraw(); }start = true; break;
		case 'd': if (_left == false) { _down = _up = _left = false; _right = true; redraw(); }start = true; break;

		}
	}
	else {
		switch (key) {
		case 'w': if (_down == false) { _down = _right = _left = false;  _up = true;  _direction_change = true; redraw(); } break;
		case 's': if (_up == false) { _up = _right = _left = false; _down = true;  _direction_change = true; redraw(); }break;
		case 'a': if (_right == false) { _down = _right = _up = false; _left = true;  _direction_change = true; redraw(); } break;
		case 'd': if (_left == false) { _down = _up = _left = false; _right = true;  _direction_change = true; redraw(); }break;

		case 27: exit(1); // Esc was pressed


		}
	}

}

void AppWindow::glutSpecial(int key, int x, int y)
{
	bool rd = true;
	const float incr = GS_TORAD(2.5f);
	const float incf = 0.05f;
}

void AppWindow::glutMouse(int b, int s, int x, int y) {}

void AppWindow::glutMotion(int x, int y) {}

void AppWindow::glutMenu(int m) {}

void AppWindow::glutReshape(int w, int h)
{
	// Define that OpenGL should use the whole window for rendering
	glViewport(0, 0, w, h);
	_w = w; _h = h;
}

// here we will redraw the scene according to the current state of the application.
void AppWindow::glutDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//if a change is detected it will rebuild
	if (_axis.changed) _axis.build(1.01f);
	if (_fruit.changed) {
		if (hit_fruit == true) {
			srand(time(NULL));
			randx = static_cast<double>(rand() % 100) / 100.0f;
			randy = static_cast<double>(rand() % 100) / 100.0f;
		}
		_fruit.build(randx, randy);
	}
	if (_snake.changed) {

		if (hit_fruit == false && _direction_change == false) {
			it = xx.begin();
			xx.insert(it, right);

			it = yy.begin();
			yy.insert(it, up);
		}

		_direction_change = false;
		if (hit_fruit == true) {
			size += 4;
			score++;
			cout << "Score + 1! Your score is now: " << score << endl;
			
			hit_fruit = false;
		}
		_snake.build(size, xx, yy, right, up);
	}
	// Define our scene transformation:
	GsMat rx, ry, stransf;
	rx.rotx(_rotx);
	ry.roty(_roty);
	stransf = rx*ry; // set the scene transformation matrix

	GsMat camview, persp, sproj;

	GsVec eye(0, 0, 2), center(0, 0, 0), up(0, 1, 0);
	camview.lookat(eye, center, up); // set our 4x4 "camera" matrix

	float aspect = 1.0f, znear = 0.1f, zfar = 50.0f;
	persp.perspective(_fovy, aspect, znear, zfar); // set our 4x4 perspective matrix

	sproj = persp * camview; // set final scene projection


							 // Draw:
	_axis.draw(stransf, sproj);
	_fruit.draw(stransf, sproj);
	_snake.draw(stransf, sproj);

	// Swap buffers and draw:
	glFlush();         // flush the pipeline (usually not necessary)
	glutSwapBuffers(); // we were drawing to the back buffer, now bring it to the front
}

bool Collision(int sizes, int SoF, float randxx, float randyy, std::vector<float> &x, std::vector<float> &y) {

	float r = 0.021f;

	//Snake collides against himself
	if (SoF == 1) {
		for (int i = 1; i < sizes; i++) {

			if (x[0] == x[i])
				if (y[0] == y[i])
					return true;
		}
		return false;
	}

	//Snake collides with fruit
	else if (SoF == 2) {
		for (int i = 0; i < sizes; i++) {
			if ((randxx + r) >= x[i] && (randxx - r) <= x[i])
				if ((randyy + r) >= y[i] && (randyy - r) <= y[i])
				return true;
		}
		return false;
	}

}

//glutIdle() function is always running in the background, as long as the program is running so will glutIdle()
//so any movements or detections should be placed in this function 
void AppWindow::glutIdle() {
	///*
	int time = glutGet(GLUT_ELAPSED_TIME);

	if (_gameover == false) {

		if (_up == true) {

			if ((time % 200) == 0) {
				up += 0.05f;
				_snake.changed = true;					//signify a change has occured
				redraw();								//redraw with change

				if (up >= 1.01) _gameover = true; // up
				if (_gameover == false) //up
					if (size > 2)
						_gameover = Collision(size, 1, 0.0f, 0.0f, xx, yy);						
				if (_gameover == false)
					if (hit_fruit == false)
						hit_fruit = Collision(size, 2, randx, randy, xx, yy); // up
			}
		}
		else if (_down == true) {

			if ((time % 200) == 0) {

				up -= 0.05f;
				_snake.changed = true;
				redraw();

				if (up <= -1.01) _gameover = true;
				if (_gameover == false)
					if (size > 2)
						_gameover = Collision(size, 1, 0.0f, 0.0f, xx, yy);
				if (_gameover == false)
					if (hit_fruit == false)
						hit_fruit = Collision(size, 2, randx, randy, xx, yy);

			}
		}
		else if (_right == true) {

			if ((time % 200) == 0) {
				right += 0.05f;
				_snake.changed = true;
				redraw();

				if (right >= 1.01) _gameover = true;
				if (_gameover == false)
					if (size > 2)
						_gameover = Collision(size, 1, 0.0f, 0.0f, xx, yy);
				if (_gameover == false)
					if (hit_fruit == false)
						hit_fruit = Collision(size, 2, randx, randy, xx, yy);

			}
		}
		else if (_left == true) {

			if ((time % 200) == 0) {
				right -= 0.05f;
				_snake.changed = true;
				redraw();

				if (right <= -1.01) _gameover = true;
				if (_gameover == false)
					if (size > 2)
						_gameover = Collision(size, 1, 0.0f, 0.0f, xx, yy);
				if (_gameover == false)
					if (hit_fruit == false)
						hit_fruit = Collision(size, 2, randx, randy, xx, yy);
			}
		}

		if (_gameover == true) {	
			cout << "Game Over! You Died!" << endl;
			cout << "Your final score was: " << score;

		}
		
	}
}