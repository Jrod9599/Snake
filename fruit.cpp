
# include "fruit.h"
# include <stdLib.h>

Fruit::Fruit()
{
	_numpoints = 0;
}

void Fruit::init(const GlProgram& prog)
{
	// Define buffers needed:
	set_program(prog);
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(2);       // will use 2 buffers: one for coordinates and one for colors
	uniform_locations(2); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");
}

//Build is where you would give the fruit its shape and its color
void Fruit::build(float x, float y)
{

	int i;
	float r = 0.0175f;
	const float d = r / 20.0f;

	P.clear(); C.clear();   // set size to zero, just in case
	P.reserve(8); C.reserve(8); // reserve space to avoid re-allocations from the calls below
	
	//top of box
	P.push_back(GsVec(-r + x, r + y, 0)); P.push_back(GsVec(r + x, r + y, 0));
	for (i = 0; i<2; i++) C.push_back(GsColor::red); // recall that GsColor has r,g,b,a values

	//left wall of box
	P.push_back(GsVec(-r + x, -r + y, 0)); P.push_back(GsVec(-r + x, r + y, 0));
	for (i = 0; i<2; i++) C.push_back(GsColor::red);

	//right wall of box
	P.push_back(GsVec(r + x, -r + y, 0)); P.push_back(GsVec(r + x, r + y, 0));
	for (i = 0; i<2; i++) C.push_back(GsColor::red);

	//bottom of box
	P.push_back(GsVec(-r + x, -r + y, 0)); P.push_back(GsVec(r + x, -r + y, 0));
	for (i = 0; i<2; i++) C.push_back(GsColor::red);

	// send data to OpenGL buffers:
	glBindBuffer(GL_ARRAY_BUFFER, buf[0]);
	glBufferData(GL_ARRAY_BUFFER, P.size() * 3 * sizeof(float), &P[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, buf[1]);
	glBufferData(GL_ARRAY_BUFFER, C.size() * 4 * sizeof(gsbyte), &C[0], GL_STATIC_DRAW);

	// save size so that we can free our buffers and later just draw the OpenGL arrays:
	_numpoints = P.size();

	// free non-needed memory:
	P.resize(0); C.resize(0);
}

//Draw function tells program to draw it on the screen
void Fruit::draw(GsMat& tr, GsMat& pr)
{
	// Draw Lines:
	glUseProgram(prog);
	glBindVertexArray(va[0]);

	glBindBuffer(GL_ARRAY_BUFFER, buf[0]); // positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, buf[1]); // colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_FALSE, 0, 0);

	glUniformMatrix4fv(uniloc[0], 1, GL_FALSE, tr.e);
	glUniformMatrix4fv(uniloc[1], 1, GL_FALSE, pr.e);

	glDrawArrays(GL_LINES, 0, _numpoints);
}

