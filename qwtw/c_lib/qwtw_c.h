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

/**  this is just for testing*/
qwtwc_API 	int get42(int n);

/**  print version info string (with 'sprintf'). warning: no vstr length check!!
	@param[out] vstr string with version info
	@return number of bytes in vstr

*/
qwtwc_API		int qwtversion(char* vstr);

/**  create (and draw) new plot with ID 'n';  make this plot 'active'
     if plot with this ID already exists, it will be made 'active'
 @param[in] n this plot ID
*/
qwtwc_API 	void qwtfigure(int n);

#ifdef USEMARBLE
/**  create (and draw) new map plot with ID 'n'.
@param[in] n this plot ID
*/
qwtwc_API 	void topview(int n);
#endif

/** put a title on currently 'active' plot.
	@param[in] s the title
*/
qwtwc_API 	void qwttitle(char* s);

/** put a 'label' on X (bottom) axis.
	@param[s] axis name string
*/
qwtwc_API 	void qwtxlabel(char* s);

/** put a 'label' on Y (left) axis.
@param[s] axis name string
*/
qwtwc_API 	void qwtylabel(char* s);

/** close all figures.
*/
qwtwc_API 	void qwtclear();

/** This function sets some additional flags on (following) lines.
	@param[in] status if '0', all next lines will be "not important":
	 this means that they will not participate in 'clipping' and after pressing "clip" button
	 thier range will not be considered.
	 '1' would return this backward.
*/
qwtwc_API 	void qwtsetimpstatus(int status);

/**  add line to the current 'active' plot.
 should 'x' and 'y' arrays exists after call to this function? I hope no. 
 @param[in] x pointer to x values
 @param[in] y pointer to y-axis values
 @param[in] size size of 'x' and 'y' array 
 @param[in] name name for this particular line
 @param[in] style line style. This is a string with 1 or 2  or 3 char:

 \code
    string, 1, 2 or 3 characters;

      last char is always color:
      'r'  red
      'd'  darkRed
      'k'  black
      'w'  white (quite useless because background is white)
      'g'  green
      'G'  darkGreen
      'm'  magenta
      'M'  darkMagenta
      'y'  yellow
      'Y'  darkYellow
      'b'  blue
      'c'  cyan
      'C'  darkCyan

      //first char is always a line style:

      ' ' NoCurve
      '-' Lines
      '%' Sticks
      '#' Steps
      '.' Dots

       //   middle char is symbol type (if we need symbols):

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


 @param[in] lineWidth line width (1 is fastest)
 @param[in] symSize  size parameter for the symbols (if we use symbols instead of lines).

*/
qwtwc_API 	void qwtplot(double* x, double* y, int size, char* name, const char* style, 
    int lineWidth, int symSize);

/**  same as above, but with additional 'time information'.
	\see qwtplot function description
	@param[in] time time info; we use this info to properly draw a marker on this plot.

	For example, we can create two plots describing some trajectory: on the first plot we output 'velocity vs time' info,
	and on second plot we output 'view from the top'; imagine we have following arrays for this trajectory:
	x coord
	y coord
	time
	x velo
	

	for first plot we'll use 'x velo' and 'time' arrays, and for second plot we'll use 'x coord', 'y coord' and 'time' arrays.
*/
qwtwc_API 	void qwtplot2(double* x, double* y, int size, char* name, const char* style, 
    int lineWidth, int symSize, double* time);

/** do not use it if all is working without it.
    This function will try to "close" QT library.  Craches sometimes.
*/
qwtwc_API 	void qwtclose(); //  works strange

/** Show 'main window' which allow to easily switch between other windows.

*/
qwtwc_API 	void qwtshowmw(); 


#ifdef __cplusplus
	}
#endif




