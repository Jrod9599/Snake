
# include <iostream>
# include <gsim/gs.h>
# include <stdlib.h>
# include "app_window.h"

AppWindow::AppWindow ( const char* label, int x, int y, int w, int h )
          :GlutWindow ( label, x, y, w, h )
 {
   initPrograms ();
   addMenuEntry ( "Option 0", evOption0 );
   addMenuEntry ( "Option 1", evOption1 );
   _viewaxis = true;
   _up = false;
   _down = false;
   _right = false;
   _left = false;
   _gameover = false;
   _fovy = GS_TORAD(60.0f);
   _rotx = _roty = 0;
   _w = w;
   _h = h;
   up = 0;
   right = 0;
   size = 2;
   hit = 0;
   test = 0.0f;

   randx = randy = 0;

   xx.push_back(0.0);
   yy.push_back(0.0);
   xx.push_back(0.05);
   yy.push_back(0);

 }

void AppWindow::initPrograms ()
 {
   // Load your shaders and link your programs here:
   _vertexsh.load_and_compile ( GL_VERTEX_SHADER, "../vsh_mcol_flat.glsl" );
   _fragsh.load_and_compile ( GL_FRAGMENT_SHADER, "../fsh_flat.glsl" );
   _prog.init_and_link ( _vertexsh, _fragsh );

   // Init my scene objects:
   _axis.init ( _prog );
   _fruit.init(_prog);
   _snake.init(_prog);
 }

// mouse events are in window coordinates, but your 2D scene is in [0,1]x[0,1],
// so make here the conversion when needed
GsVec2 AppWindow::windowToScene ( const GsVec2& v )
 {
   // GsVec2 is a lighteweight class suitable to return by value:
   return GsVec2 ( (2.0f*(v.x/float(_w))) - 1.0f,
                    1.0f - (2.0f*(v.y/float(_h))) );
 }

// Called every time there is a window event
void AppWindow::glutKeyboard ( unsigned char key, int x, int y )
 {
   switch ( key )
    { 
		case ' ': _viewaxis = !_viewaxis; redraw(); break;
		case 'w': _down = _right = _left = false; _up = true; redraw(); break;
		case 's': _up = _right = _left = false; _down = true; redraw(); break;
		case 'a': _down = _right = _up = false; _left = true; redraw(); break;
		case 'd': _down = _up = _left = false; _right = true; redraw(); break;
		
		//case 'w': up += 0.10; redraw(); break;
		//case 's': up -= 0.10; redraw(); break;
		//case 'a': right -= 0.10; redraw(); break;
		//case 'd': right += 0.10; redraw(); break;
		
		case 'h': hit = 1; redraw(); break;
		case 27 : exit(1); // Esc was pressed


	}
 }

void AppWindow::glutSpecial ( int key, int x, int y )
 {
   bool rd=true;
   const float incr=GS_TORAD(2.5f);
   const float incf=0.05f;
   switch ( key )
    { case GLUT_KEY_LEFT:      _roty-=incr; break;
      case GLUT_KEY_RIGHT:     _roty+=incr; break;
      case GLUT_KEY_UP:        _rotx-=incr; break;
      case GLUT_KEY_DOWN:      _rotx+=incr; break;
      case GLUT_KEY_PAGE_UP:   _fovy-=incf; break;
      case GLUT_KEY_PAGE_DOWN: _fovy+=incf; break;
      case GLUT_KEY_HOME:      _fovy=GS_TORAD(60.0f); _rotx=_roty=0; break;
      default: return; // return without rendering
	}
   if (rd) redraw(); // ask the window to be rendered when possible
 }

void AppWindow::glutMouse ( int b, int s, int x, int y )
 {
   // The mouse is not used in this example.
   // Recall that a mouse click in the screen corresponds
   // to a whole line traversing the 3D scene.
 }

void AppWindow::glutMotion ( int x, int y )
 {
 }

void AppWindow::glutMenu ( int m )
 {
   std::cout<<"Menu Event: "<<m<<std::endl;
   std::cout << "W = Up \n S = Down \n A = Left \n D = Right \n Hit any direction to begin";
   //std::cout<<
 }

void AppWindow::glutReshape ( int w, int h )
 {
   // Define that OpenGL should use the whole window for rendering
   glViewport( 0, 0, w, h );
   _w=w; _h=h;
 }

// here we will redraw the scene according to the current state of the application.
void AppWindow::glutDisplay ()
 {
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   //if a change is detected it will rebuild
   if ( _axis.changed ) _axis.build(1.01f); 
   if (_fruit.changed) _fruit.build(randx, randy);
   if (_snake.changed) {
	
	   it = xx.begin();
	   xx.insert(it, right);

	   it = yy.begin();
	   yy.insert(it, up);

	   if (hit == 1) {
		   size += 4;
		   hit = 0;
	   }
	   _snake.build(size, xx, yy, right, up);
   }
   // Define our scene transformation:
   GsMat rx, ry, stransf;
   rx.rotx ( _rotx );
   ry.roty ( _roty );
   stransf = rx*ry; // set the scene transformation matrix

   GsMat camview, persp, sproj;

   GsVec eye(0,0,2), center(0,0,0), up(0,1,0);
   camview.lookat ( eye, center, up ); // set our 4x4 "camera" matrix

   float aspect=1.0f, znear=0.1f, zfar=50.0f;
   persp.perspective ( _fovy, aspect, znear, zfar ); // set our 4x4 perspective matrix

   sproj = persp * camview; // set final scene projection

  
   // Draw:
   if ( _viewaxis ) _axis.draw ( stransf, sproj );
   //_fruit.draw(stransf, sproj);
   _snake.draw(stransf, sproj);
  
   // Swap buffers and draw:
   glFlush();         // flush the pipeline (usually not necessary)
   glutSwapBuffers(); // we were drawing to the back buffer, now bring it to the front
}

bool Collision(int sizes, int SoF, float randx, float randy, std::vector<float> &x, std::vector<float> &y) {
	//Snake collides against himself
	if (SoF == 1) {

		for (int i = 1; i < sizes; i++) {

			if (x[1] == x[i]) {
				
				for (int j = 1; j < sizes; j++) {
					if (y[1] == y[j])
						return true;
				}
			}
			
		}
		return false;

	}
	//Snake collides with fruit
	else if (SoF == 2) {

		for (int i = 0; i < sizes; i++) {

			if (randx == x[i]) {
				if (randy == y[i])
					return true;
			}
				
		}
		return false;
	}
	else
		return false;
}


//glutIdle() function is always running in the background, as long as the program is running so will glutIdle()
//so any movements or detections should be placed in this function 
void AppWindow::glutIdle() {

	int time = glutGet(GLUT_ELAPSED_TIME);
	
	if (_gameover == false) {
		if (_up == true) {

			if ((time % 200) == 0) {
				up += 0.05;
				_snake.changed = true;					//signify a change has occured
				redraw();								//redraw with change

				if (up >= 1.01) _gameover = true;
				if(_gameover == false)
					_gameover = Collision(size, 1, 0.0f,0.0f,xx,yy);
				if (_gameover = false)
					_gameover = Collision(size, 2, randx, randy, xx, yy);
			}
		}
		else if (_down == true) {

			if ((time % 200) == 0) {
				
				up -= 0.05;
				_snake.changed = true;
				redraw();

				if (up <= -1.01) _gameover = true;
				if (_gameover == false)
					_gameover = Collision(size, 1, 0.0f, 0.0f, xx, yy);
				if (_gameover = false)
					_gameover = Collision(size, 2, randx, randy, xx, yy);
			}

		}
		else if (_right == true) {

			if ((time % 200) == 0) {
				right += 0.05;
				_snake.changed = true;
				redraw();

				if (right >= 1.01) _gameover = true;
				if (_gameover == false)
					_gameover = Collision(size, 1, 0.0f, 0.0f, xx, yy);
				if (_gameover = false)
					_gameover = Collision(size, 2, randx, randy, xx, yy);

			}
		}
		else if (_left == true) {

			if ((time % 200) == 0) {
				right -= 0.05;
				_snake.changed = true;
				redraw();

				if (right <= -1.01) _gameover = true;
				if (_gameover == false)
					_gameover = Collision(size, 1, 0.0f, 0.0f, xx, yy);
				if (_gameover = false)
					_gameover = Collision(size, 2, randx, randy, xx, yy);
			}
		}
	}

}