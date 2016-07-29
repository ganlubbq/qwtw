
#include "qwtw_c.h"
#include "xstdef.h"

#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include <math.h>
#include "xmatrixplatform.h"
#ifdef WIN32
    #include <conio.h>
    #include <ctype.h>
#else // linux?
//    #include <ncurses.h>
#endif
//#include <boost/thread.hpp>
//#include <boost/chrono/thread_clock.hpp>

//#include "BSpline.h"
//#include "spline.h"


int main(int argc, char* argv[]) {
	int n = 0, ch;
	

    //  1.  create data:
	const int n1 = 40, nc = 361, n4 = 4000;
	const int n3 = 40;
    const double maxIks = 10.;
  //  boost::this_thread::sleep(boost::posix_time::milliseconds(80));

	double x1[n1], y1[n1];
	double cx[nc], cy[nc], cR = 1.4;
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
	//BSpline<double> s(x3, n3, y3, 0.0, 1);
	//tk::spline s1;
	//std::vector<double> X(x3, x3+n3), Y(y3, y3+n3);


	//s1.set_points(X, Y);

	//for (i = 0; i < nns; i++) {
//		xs[i] = x3[0] + ((double)(i)) * delta / ((double)(nns - 1));
//		ys1[i] = s1(xs[i]);
//	}
	//qwtplot(xs, ys, nns, "spline plot", "-b", 1, 1);
//	qwtplot(xs, ys1, nns, "spline1 plot", "-m", 1, 1);


//	qwttitle("QWT stest1 testing");
//	qwtxlabel("[seconds ?]");
//	qwtylabel("sinuses %)");
//	qwttitle("title");

/*	qwtfigure(1);
	qwtplot(x4, y4, n4, "x4 plot", "-r", 2, 1);
	qwtxlabel("[seconds ?]");
	qwtylabel("sinuses %)");
	qwttitle("title");

	qwtfigure(2);
	qwtplot(x4, y5, n4, "x5 plot", "-k", 2, 1);
	qwtxlabel("[seconds ?]");
	qwtylabel("sinuses %)");
	qwttitle("title");


	qwtfigure(3);
	qwtplot(x4, y6, n4, "x6 plot", "-b", 22, 1);
	qwtxlabel("[seconds ?]");
	qwtylabel("sinuses %)");
	qwttitle("title");
	*/
	qwtmode(3);
	qwtfigure(10);
	qwtplot2(cx, cy, nc, "circle", "-m", 2, 1, ct);
	qwttitle("mode #3 test");



#ifdef USEMARBLE
	const int mwn = 4;
	qwtmode(3);
	topview(18);
	double north[mwn] = {55.688713, 55.698713, 55.678713, 55.60};
	double east[mwn] = {37.901073, 37.911073, 37.905073, 37.9};
	double t4[mwn]; t4[0] = x1[0];  t4[2] = x1[int(n1 / 3.)]; t4[3] = x1[int(n1 / 2.)]; t4[4] = x1[n1 - 1];

	for (int i = 0; i < mwn; i++) {
		//east[i] *= Deg2Rad;
		//north[i] *= Deg2Rad;
	}
	qwtplot2(east, north, mwn, "test mw", "-b", 2,  3, t4);

	qwttitle("marble top view");
#endif

	qwtshowmw();
		
  //  ch = getch();
	int itmp;
	std::cin >> itmp;

//	qwtclose();
	delete[] x3;
	delete[] y3;
	delete[] x4;
	delete[] y4;
	delete[] y5;
	delete[] y6;

	return 0;
}



