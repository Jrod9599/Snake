
// Ensure the header file is included only once in multi-file projects
#ifndef APP_WINDOW_H
#define APP_WINDOW_H

# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include <ctime>
# include "ogl_tools.h"
# include "glut_window.h"
# include "so_axis.h"
# include "fruit.h"
# include "Snake.h"


// The functionality of your application should be implemented inside AppWindow
class AppWindow : public GlutWindow
 { 
 private :
    
	 // OpenGL shaders and programs:
    GlShader _vertexsh, _fragsh;
    GlProgram _prog;
    
	// My scene objects:
    SoAxis _axis;
	Fruit  _fruit;
	Snake  _snake;
	
    // App data:
    enum MenuEv { evOption0, evOption1 };
    float _rotx, _roty, _fovy, up, right, test;
    bool  _viewaxis, _viewsnake;
	bool _up,_down,_left, _right, _direction_change, _gameover, start, hit_fruit;
    int _w, _h, size;

	float randx, randy;

	std::vector<float> xx;
	std::vector<float> yy;
	std::vector<float>::iterator it;

   public :
    AppWindow ( const char* label, int x, int y, int w, int h );
    void initPrograms ();
    GsVec2 windowToScene ( const GsVec2& v );

   private : // functions derived from the base class
    virtual void glutMenu ( int m );
    virtual void glutKeyboard ( unsigned char key, int x, int y );
    virtual void glutSpecial ( int key, int x, int y );
    virtual void glutMouse ( int b, int s, int x, int y );
    virtual void glutMotion ( int x, int y );
    virtual void glutDisplay ();
    virtual void glutReshape ( int w, int h );
	virtual void glutIdle();
 };

#endif // APP_WINDOW_H
