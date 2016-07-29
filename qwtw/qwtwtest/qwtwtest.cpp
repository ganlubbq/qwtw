
#include "qwtw_c.h"
#include "xstdef.h"

#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include <math.h>
#ifdef WIN32
    #include <conio.h>
    #include <ctype.h>
#else // linux?
    #include <ncurses.h>
#endif

void assert_failed(const char* file, unsigned int line, const char* str) {

}

int main(int argc, char* argv[]) {
	int n = 0, ch;
	

    //  1.  create data:
	const int n1 = 40, nc = 361, n4 = 4000;
	const int n3 = 40;
    const double maxIks = 10.;

	double x1[n1], y1[n1];
	double cx[nc], cy[nc], cx2[nc], cy2[nc], cR = 1.4, cR2 = 1.1;
	double ct[nc];

	double* x3 = new double[n3];
	double* y3 = new double[n3]; 

	double* x4 = new double[n4];
	double* y4 = new double[n4];
	double* y5 = new double[n4];
	double* y6 = new double[n4];
	
	int i;
	for (i = 0; i < n1; i++) {
		x1[i] = i * maxIks / n1; 
		y1[i] = sin(x1[i]);
	}

	for (i = 0; i < nc; i++) {
		ct[i] = i * 3.14159 / 180.;

		cx[i] = sin(ct[i])* cR;
		cy[i] = cos(ct[i]) * cR;
		cx2[i] = sin(ct[i])* cR2;
		cy2[i] = cos(ct[i]) * cR2;

	}


	for (i  =  0; i < n3; i++) {
		x3[i] = (i*maxIks) / n3;
		y3[i] = cos(x3[i]*0.9) * sin(x3[i]*1.1);
	}

	for(i = 0; i < n4; i++) {
	    x4[i] = (i*maxIks) / n4;
	    y4[i] = cos(x4[i] * 0.9) + sin(x4[i] * 1.1);
	    y5[i] = cos(x4[i] * 0.4) - sin(x4[i] * 1.5);
	    y6[i] = cos(x4[i] * 0.2) + sin(x4[i] * 1.15);
	}

    //  2. draw everything:
    qwtfigure(14);
    qwtmode(2);
	qwtxlabel("x label");
	qwtylabel("Y label");
	qwttitle("title");

	qwtplot(x1, y1, n1, "x1 plot", "-b", 2, 1);
	qwtplot(cx, cy, nc, "circle", "-m", 2, 1);

	//qwtplot(x1, y2, n1, "x2 plot", "-m", 2, 1);
	//qwtplot(x1, y4, n1, "x3 plot", "-M", 2, 1);
	
    qwtfigure(7);


    qwtplot(x3, y3, n3, "x3 plot", "-r", 1, 1);

    qwttitle("QWT stest1 testing");
    qwtxlabel("[seconds ?]");
    qwtylabel("sinuses %)");

	const int nns = 10000;
	double xs[nns], ys[nns], ys1[nns];
	double delta = x3[n3 - 1] - x3[0];
	qwtmode(3);
	qwtfigure(10);
	qwtplot2(cx, cy, nc, "circle", "-m", 2, 1, ct);
	qwtplot2(cx2, cy2, nc, "circle 2", "-r", 2, 1, ct);
	qwttitle("mode #3 test");


	/*
	topview(16);
	double x9[4]; 
	double y9[4]; 
	double t9[4];
	x9[0] = 37.6519;	y9[0] = 55.72218;		t9[0] = 1.;
	x9[1] = 37.9;		y9[1] = 56.08;			t9[1] = 2.;
	x9[2] = 38.;		y9[2] = 56.5;			t9[2] = 3.;
	x9[3] = 38.2;		y9[3] = 55.5;			t9[3] = 4.;

	qwtplot2(x9, y9, 4, "test #16", "-r", 4, 1, t9);
	qwtplot2(x9, y9, 4, "test #16#3", "-b", 4, 1, t9);
	qwttitle("test #16 _ 1");

	*/

    ch = getch();
	int itmp;
	std::cin >> itmp;
	/*
	topview(16);
	qwtplot2(x9, y9, 4, "test #16 _ 2", "-b", 4,1, t9);
	qwttitle("test #16");

	xm_printf("enter something to exit\n");
	std::cin >> itmp;
	ch = getch();
	*/
//	qwtclose();
	delete[] x3;
	delete[] y3;
	delete[] x4;
	delete[] y4;
	delete[] y5;
	delete[] y6;

	return 0;
}

