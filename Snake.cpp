
# include "Snake.h"

Snake::Snake()
{
	_numpoints = 0;
}

void Snake::init(const GlProgram& prog)
{
	// Define buffers needed:
	set_program(prog);
	gen_vertex_arrays(1); // will use 1 vertex array
	gen_buffers(2);       // will use 2 buffers: one for coordinates and one for colors
	uniform_locations(2); // will send 2 variables: the 2 matrices below
	uniform_location(0, "vTransf");
	uniform_location(1, "vProj");
}

void Snake::build(int size, std::vector<float>& x, std::vector<float>& y, float right, float up )
{
	//int i;
	float r = 0.01f;
	const float d = r / 20.0f;
	const float depth = 0.0;

	ranX = 1.0; //(rand() % 100) / 100;
	ranY = 2.0;//(rand() % 100) / 100;

	P.clear(); C.clear();   // set size to zero, just in case
	

	for (int i = 0; i < size; i++) {

		if (i == 0) {
			P.push_back(GsVec(right,up, 0.0f));
			P.push_back(GsVec(x[i], y[i], 0.0f));
			for (int i = 0; i < 2; i++) C.push_back(GsColor::lightgray);
		}
		else {

			P.push_back(GsVec(x[i-1], y[i-1], 0.0f));
			P.push_back(GsVec(x[i], y[i], 0.0f));
			for (int i = 0; i < 2; i++) C.push_back(GsColor::lightgray);
		}
	}

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

void Snake::draw(GsMat& tr, GsMat& pr)
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

