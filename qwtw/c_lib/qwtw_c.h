/**
QWT-based 2D plotting library.

*/


#pragma once

#ifdef WIN32
#ifdef qwtwcEXPORTS	
#define qwtwc_API __declspec(dllexport) // Q_DECL_EXPORT 
#else
#define qwtwc_API  __declspec(dllimport) // Q_DECL_IMPORT
#endif
#else
#define qwtwc_API
#endif


#ifdef __cplusplus
	extern "C" {
#endif

qwtwc_API 	int get42(int n);
qwtwc_API		int qwtversion(char* vstr);

/**  create (and draw) new plot with ID 'n';  make this plot 'active'
     if plot with this ID already exists, it will be made 'active'
 @param[in] n this plot ID

*/
qwtwc_API 	void qwtfigure(int n);
#ifdef USEMARBLE
qwtwc_API 	void topview(int n);
#endif

/** put a title on 'active' plot

*/
qwtwc_API 	void qwttitle(char* s);

/** put 'label' on X axis

*/
qwtwc_API 	void qwtxlabel(char* s);

/** put 'label' on Y axis

*/
qwtwc_API 	void qwtylabel(char* s);

/** set current plot mode.
    	0 - do not use markers
	1 - draw 'vertical line' markers using "x" (y[i] - y[i-1] ==  const)
	2 - draw 'vertical line' markers using "x" (y[i] - y[i-1] !=  const)
	3 - draw 'point' marker using "time"

*/
qwtwc_API 	void qwtmode(int mode);

/**
close all figures;
*/
qwtwc_API 	void qwtclear();

/**
	if 'false', all next lines will be "not important":
	 - they will not participate in 'clipping'
*/
qwtwc_API 	void qwtsetimpstatus(int status);

/**  add line to the current 'active' plot
 should 'x' and 'y' arrays exists after call to this function? I hope no. but ....
 @param[in] x pointer to x values
 @param[in] y y-axis values
 @param[in] size size of 'x' and 'y' array
 @param[in] name name of this particular line
 @param[in] style line style.

 \code
    string, 1, 2 or 3 characters;

      last is always color:
      'r'  red
      'd'  darkRed
      'k'  black
      'w'  white
      'g'  green
      'G'  darkGreen
      'm'  magenta
      'M'  darkMagenta
      'y'  yellow
      'Y'  darkYellow
      'b'  blue
      'c'  cyan
      'C'  darkCyan

      first is always a line style:

      ' ' NoCurve
      '-' Lines
      '%' Sticks
      '#' Steps
      '.' Dots

       //   middle is symbol type:

       'e' Ellipse
       'r' Rect
       'd' Diamond
       't' Triangle
       'x' Cross
       's' Star1
       'q' Star2
       'w' XCross
       'u' UTriangle

 \endcode


 @param[in] lineWidth line width (in pixels?) (1 is fastest)
 @param[in] symSize  size of the symbols (if we use symbols instead of lines).

*/
qwtwc_API 	void qwtplot(double* x, double* y, int size, char* name, const char* style, 
    int lineWidth, int symSize);

/**  same as above, but with additional time information

*/
qwtwc_API 	void qwtplot2(double* x, double* y, int size, char* name, const char* style, 
    int lineWidth, int symSize, double* time);

/** do not use it if all is working without it.
    This function will try to "close" QT library.
*/
qwtwc_API 	void qwtclose(); //  works strange

qwtwc_API 	void qwtshowmw(); 


#ifdef __cplusplus
	}
#endif




