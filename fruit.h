
// Ensure the header file is included only once in multi-file projects
#ifndef FRUIT_H
#define FRUIT_H

// Include needed header files
# include <vector>
# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"

// Scene objects should be implemented in their own classes; and
// here is an example of how to organize a scene object in a class.
// Scene object axis:
class Fruit : public GlObjects
{
private:
	std::vector<GsVec>   P; // coordinates
	std::vector<GsColor> C; // colors
	int _numpoints;         // saves the number of points
	double ranX, ranY;

public:
	Fruit();
	void init(const GlProgram& prog);
	void build(float x, float y);
	void draw(GsMat& tr, GsMat& pr);
};

#endif // FRUIT_H
