/** Matrix library.
	It doesn't not use new , delete, malloc  etc   functions.

	you may define following MACROS: \n
	\code
	- WIN32			used in case Windows OS compilation
	- LIN_UX			linux
	- XM_ARM			STM32F4 ARM Coretex M4 - use 'float' , not 'XMType'
	- XM_ARM_M0			ARM Cortex M0 - will not work, no FPU!
	- MATLAB_MEX_FILE		when compilation with MATLAB (mex or mexw32 files)
	- SIMULINK			when compilation with MATLAB-Simulink (mexw32 files)
	- PCM9362      for GI2   linux box
	- _DEBUG			debug version (bounds check etc  implemented)

	see xstdef.h file for more information
	\endcode


	\file xmatrix2.h
	\author   Igor Sandler
	\date    nov 2006
	\version 1.0
	
*/

#ifndef MATRIX2_H_FILE
#define MATRIX2_H_FILE

#include "xmatrixplatform.h"
#include "xmatrix_imp.h"
#ifdef USE_GLS_LIBRARY
#include "xmgsl.h"
#endif


#undef max
#undef min
// ####################################################################################
// ####################################################################################
// ####################################################################################
// ####################################################################################



/** \class IMatrix
	Base class - the matrix.     \n
\n
	\par usage examples
	Create a matrix \a m1, \a m2, \a b1, \a b2 \a c, 4x4:
	\code 
		IMatrix<4, 4> m1;
		IMatrix<4, 4> m2;
		IMatrix<4, 4> b1;
		IMatrix<4, 4> b2;
		IMatrix<4, 4> c;
  	\endcode

	Fill \a m1 and \a m2 with random numbers. b1 = m1 + m2;  b2 = m1 * m2;
	\code 
		m1.fillrnd(); m2.fillrnd();
		b1 = m1 + m2; 
		b2 = m1 * m2;
  	\endcode
	
	Print all matrices:
	\code 
		m1.print();   m2.print();	b1.print();   b2.print();
  	\endcode
	
	Save \a b2 in \a b1, and invert \a b2 matrix:
	(may be \a b2 can not be inverted)
	\code 
		b1 = b2;
		b2.invSelf();
  	\endcode

	c = b1 * b2.   Should be "EYE" matrix:
	\code 
		c = b1 * b2;
		c.print();
  	\endcode

  */

template < iiType hh, iiType  ww>	class IMatrix : public IMatrixImpl {

public:
	XMType x[hh*ww]; ///<   our matrix; it's public, use it if you want to.

	/**   default constructor:
		@param _name used for debugging only
	*/
	IMatrix(const char* _name = 0) {
		init(_name);
	}

	/**  clear all the matrix; set zeros everywhere */
	inline void empty() {
		memset(x, 0, hh*ww*XMTSize);
	}
     
	// ====================================================================================
	/** Constructor for initialization the matrix object with another matrix object.
	*   and for copying objects when passing or returning by value.
	*	You can assign new name to the matrix.
	*/
	IMatrix(const IMatrix<hh, ww>& m, const char* _name = 0) : IMatrixImpl() {
		init(_name);
#ifdef _DEBUG   
		if (name[0] == 0) {  //  no "_name", copy it from source:
			strncpy(name, m.name, NAMELEN_DBG);
		}
 #endif
		memcpy(x, m.x, hh*ww*XMTSize);
	}

	// ====================================================================================
	/**   create a new matrix from one XMType value.
	@param[in] d all matrix elements will be equal to this value
	*/
	IMatrix(XMType d, const char* _name = 0) : IMatrixImpl() {
		init(_name);
#ifdef _DEBUG   
		if (name[0] == 0) {  //  no "_name", copy it from source:
			strncpy(name, "from_DBL\0", NAMELEN_DBG);
		}
 #endif
		iiType i;
		for (i = 0; i < hh*ww; i++) {
			x[i] = d;
		}
	}

	/**   create a new matrix from array of XMType values.
	@param[in] a all matrix elements will be equal to this array
	*/
	 IMatrix(const XMType a[hh][ww], const char* _name = 0) : IMatrixImpl() {
		init(_name);
		memcpy(x, a,  hh*ww*XMTSize);
	}


	// ====================================================================================
#ifdef MATLAB_MEX_FILE
	/**	create a matrix from matlab variable */
	IMatrix(const mxArray* mx, const char* _name = 0) {
		init(_name);
		IMatrixImpl::mxInit(mx);
	}
	void toMXArray(mxArray* mx) {
		IMatrixImpl::toMXArray(mx);
	}
#endif

	// ====================================================================================
#ifdef SIMULINK 
	/**	create a matrix from simulink input port */
	IMatrix(SimStruct *S, iiType port, char* _name = 0) {
		init(_name);
		IMatrixImpl::portInit(S, port);
	}
	void output(SimStruct *S, iiType port) { IMatrixImpl::output(S, port); }
#endif

	// ====================================================================================
	/**  extract 1 element from a 2D  matrix.
		\b example:
		\code
		M(i, j) = 57.0; // correct
		\endcode
		@param[in] i	row
		@param[in] j	column
		@return matrix element
	*/
	inline XMType& operator()(const iiType i, const iiType j)  { 
		mxassert((i < hh) && (j < ww), "bounds error in IMatrix::operator() #1 \n");
		return x[i*ww+j]; // in Release may be no bounds check!!
	}

	inline XMType operator()(const iiType i, const iiType j) const { 
		mxassert((i < hh) && (j < ww), "bounds error in IMatrix::operator() #2 \n");
		return x[i*ww+j]; // in Release may be  no bounds check!!
	}


	// ====================================================================================
	/**	extract one element from a 1D  matrix.
		@param[in] i index
	*/ 
	inline XMType& operator()(const iiType i)  {
		mxassert((i < ww*hh), "IMatrix XMType operator()(unsigned short i)): #1 index out of bounds; \n");
		return x[i];  //  in Release   no bounds check!!
	}
	/**	extract one const element from a 1D  matrix.
	*/
	inline XMType operator()(const iiType i) const  {
		mxassert((i < ww*hh), "IMatrix XMType operator()(unsigned short i)): #2 index out of bounds; \n");
		return x[i];  //  in Release   no bounds check!!
	}
	
	// ====================================================================================
	// 		get one  element from 1D matrix. 
	//	same as () operator
	//
	inline XMType& operator[](const iiType i)  {
		mxassert((i < ww*hh), "IMatrix XMType operator[](const iiType i) const  index out of bounds; \n");
		return x[i];
	}
	//		get one  element from 1D matrix. 
	//	same as () operator	
	//	
	inline XMType operator[](const iiType i) const  {
		mxassert((i < ww*hh), "IMatrix XMType operator[](const iiType i) const  index out of bounds; \n");
		return x[i];
	}
	
	/** return j'th column from this matrix

	*/
	IMatrix<hh, 1> col(int j) const {
		IMatrix<hh, 1> ret;
		mxassert(j < ww, "");
		for(int i = 0; i < hh; i++) {
			ret[i] = x[i*ww + j];
		}
		return ret;
	}


	// ====================================================================================
	/**--	Overloaded matrix appropriation operator.	
	*/
	IMatrix<hh, ww>& operator=(const IMatrix<hh, ww> &m){
		// In case the object is assigned to itself:
		if (this == &m) return *this;
#ifdef _DEBUG  
		//  if name absent, take if from another matrix
		if (name[0] == 0) {
			memcpy(name, m.name, NAMELEN_DBG + 1);
		}
#endif
		memcpy(x, m.x, hh*ww*XMTSize); //    this should be fast, depending on platform
		return *this;
	}

	// ====================================================================================
	/** set matrix equal to XMType value. 
	*/
	IMatrix<hh, ww>& operator=(XMType d) {
		iiType i;
		for (i = 0; i < ww*hh; i++) {
			x[i] = d;
		}
		return *this; 
	}

	//operator XMType*() {
	//	return x;
	//}
	
	// ====================================================================================
	/**	create new 1D  matrix from part of another 1D matrix.	
	@param[in] from element in another matrix; we will copy info from another matrix 
			   (starting from 'from' index) to our new matrix.
	*/
	template < iiType h, iiType  w> IMatrix(const IMatrix<h, w>& m, iiType from, const char* _name = 0) {
		mxassert(from + hh*ww <= h*w, " ");
		init(_name);
		memcpy(x, m.x + from, hh*ww*XMTSize);
	}

	// ====================================================================================
	/**	create a 1D  matrix from part of another 1D matrix.
	*/
	template < iiType h, iiType  w> void take(const IMatrix<h, w> &m, iiType from) {
		mxassert(from + hh*ww <= h*w, " ");
		memcpy(x, m.x + from, hh*ww*XMTSize);
	}

	/**	create a 1D  matrix from XMType array.
	this is dangerous function, no bounds check.
	*/
	void take(const XMType* v) {
		memcpy(x, v, hh*ww*XMTSize);
		//for (iiType i = 0; i < size; i++) x[i] = v[i];
	}
	/**	create a 1D  matrix from XMType array.
	*/

	void take(const XMType v[hh][ww]) {
		memcpy(x, v, hh*ww*XMTSize);
	}


	IMatrix(const XMType* v) {
		init(0);
		memcpy(x, v, hh*ww*XMTSize);
	}

	/** create a matrix from  XMType array.
	 So, we can send  XMType arrays to functions, which expect IMatrix as input.
	*/
	template < iiType h> IMatrix(const XMType v[h]) {
		mxassert(hh*ww >= h, " bounds check error #72634 ");
		init(0);
		memcpy(x, v, h*XMTSize);
	}

	// do not look at the code below
	IMatrix(XMType q1, XMType q2) {
		mxassert(hh*ww >= 2, "");
		init(0);
		x[0] = q1; 
		x[1] = q2; 
	}

	IMatrix(XMType q1, XMType q2, XMType q3) {
		mxassert(hh*ww >= 3, "");
		init(0);
		x[0] = q1; 
		x[1] = q2; 
		x[2] = q3;
	}

	IMatrix(XMType q1, XMType q2, XMType q3, XMType q4) {
		mxassert(hh*ww >= 4, "");
		init(0);
		x[0] = q1;
		x[1] = q2;
		x[2] = q3;
		x[3] = q4;
	}
	IMatrix(XMType q1, XMType q2, XMType q3, XMType q4, XMType q5) {
		mxassert(hh*ww >= 5, "");
		init(0);
		x[0] = q1; 
		x[1] = q2; 
		x[2] = q3;
		x[3] = q4; 
		x[4] = q5; 
	}

	// ====================================================================================
	/**	create a 2D  matrix from part of another 2D matrix.
		@param m   src matrix
		@param r1 start row
		@param c1  start column

		TODO: we can do this more optimal way
	*/
	template <iiType h, iiType w> IMatrix(const IMatrix<h, w>& m, 
			 iiType r1, 	iiType c1, 		const char* _name = 0) {
		
		iiType r2 = r1 + hh - 1;
		mxassert(((r2 < h) && (c1 + ww - 1 < w)), " ");
		init(_name);
		iiType i;
		for (i = r1; i <= r2; i++) {
			memcpy(x + (i-r1)*ww, m.x + i*w + c1, ww*XMTSize);
		}
	}

	// ====================================================================================
	/**	create a 2D  matrix from part of another 2D matrix.
		@param m   src matrix
		@param r1 start row
		@param c1  start column

		TODO: we can do this more optimal way
	*/
	template < iiType h, iiType  w> void take2(const IMatrix<h, w> &m, 
			 iiType r1, 		 iiType c1) {
		
		iiType r2 = r1 + hh - 1;
		mxat(((r2 < h) && (c1 + ww - 1 < w)));
		iiType i;
		for (i = r1; i <= r2; i++) {
			memcpy(x + (i-r1)*ww, m.x + i*w + c1, ww*XMTSize);
		}
	}

	/** take part of another matrix, and put it somewhere into 'this' matrix
	@param[in] m 'another matrix'
	@param r1  'another matrix' 'start' raw
	@param c1 'another matrix' 'start' column
	@param w1 how many columns we need to take from 'another matrix' m
	@param h1 how many rows we need to take from 'another matrix' m
	@param r destination raw
	@param c destination column

	*/
	template < iiType h, iiType  w> void take4(const IMatrix<h, w> &m,
		int r1, int c1, int h1, int w1, int r, int c) {
		
		mxat((r1+h1 <= h) && (c1+w1 <= w));
		mxat((r + h1 <= hh) && (c + w1 <= ww));

		for (int i = 0; i < h1; i++) {
			memcpy(x + (r + i)*ww + c, m.x + (r1 + i)*w + c1, w1*XMTSize);
		}
	}


	void ftake(const float* a) {
		for(int i = 0; i < hh*ww; i++) {
			x[i] = a[i];
		}
	}

	void putf(float* a) const {
		for(int i = 0; i < hh*ww; i++) {
			a[i] = x[i];
		}
	}

	/**  put everything inside 'v'

	*/
	void put(XMType* v) {
	    memcpy(v, x, hh*ww*sizeof(XMType));
	}
	// ====================================================================================
	/**
			Overloaded matrix addition operator.			
	*/
	/*
	IMatrix<hh, ww> operator+(const IMatrix<hh, ww> &m) const{
		IMatrix<hh, ww> m2;
		IMatrixImpl::add(m2.x, x, m.x, hh*ww);
		return m2;
	}
	*/

	inline IMatrix<hh, ww> operator+(const IMatrix<hh, ww>& m) const {
		IMatrix<hh, ww> m2;
		const int n = hh*ww;
		for(int i = 0; i < n; i++) {
			m2.x[i] = x[i] + m.x[i];
		}
		return m2;
	}


	// ====================================================================================
	IMatrix<hh, ww>& operator+=(const IMatrix<hh, ww> &m)  {
		iiType i;
		const int n = hh*ww;
		for(i = 0; i < n; i++) {
			x[i] += m.x[i];
		}
		return *this;
	}

	// ====================================================================================
	IMatrix<hh, ww>& operator-=(const IMatrix<hh, ww> &m)  {
		iiType i;
		const int n = hh*ww;
		for(i = 0; i < n; i++) {
			x[i] -= m.x[i];
		}
		return *this;
	}

	// ====================================================================================
	/*
	IMatrix<hh, ww> operator-(const IMatrix<hh, ww> &m)   const {
		IMatrix<hh, ww> m2;
		IMatrixImpl::sub(m2.x, x, m.x, hh*ww);
		return m2;
	}
	*/

	inline IMatrix<hh, ww> operator-(const IMatrix<hh, ww>& m) const {
		IMatrix<hh, ww> m2;
		int  i;
		const int n = hh*ww;
		for(i = 0; i < n; i++) {
			m2.x[i] = x[i] - m.x[i];
		}
		return m2;
	}



	// ====================================================================================
	/**    unary minus.
	*/
	IMatrix<hh, ww> operator-() const {
		IMatrix<hh, ww> m2;
		const int n = hh*ww;
		//IMatrixImpl::minus(m2.x, x, hh*ww);
		for(int i = 0; i < n; i++) {
			m2.x[i] = -x[i];
		}

		return m2;
	}
	//   square norm for a vector.
	inline XMType norma2() const {  
		return IMatrixImpl::norma2(x, hh*ww); 
	}
	inline void normalize()  {   
		IMatrixImpl::normalize(x, hh*ww); 
	}

	/**
		@return vector length (sqrt(norma2()))
	*/
	inline XMType length() const {   
		return sqrt(IMatrixImpl::norma2(x, hh*ww)); 
	}


	// ====================================================================================
	/**
			Overloaded matrix multiplication operator.	
			w: width of the second matrix.
	*/
	template <iiType w> IMatrix<hh, w> operator*(const IMatrix<ww, w>& m) const {
		IMatrix<hh, w> m2; // result;
		IMatrixImpl::mul(m2.x, x, m.x, hh, ww, w);
		return m2;
	}

	/** this = m * this 
	for square matrix
	*/
//	void  multLeft(const IMatrix<ww, w>& m) const {
//		int i, j, k;
//		for(i = 0; i < )
//	}


	// ====================================================================================
	/**
		scalar product.
		@return scalar product of two vectors
	*/
	static XMType sp(const IMatrix<hh, ww>& m1, const IMatrix<hh, ww>& m2) {
		XMType p = ZERO;
		for ( iiType i = 0; i < hh*ww; i++) {
			p += m1.x[i] * m2.x[i];
		}
		return p;
	}

	// ====================================================================================
	/**
		matrix transposition operator.
	*/
	/*
	IMatrix<ww, hh> operator!()  const  {
		IMatrix<ww, hh> dst;
		transpose(dst.x, x, hh, ww);
		return dst;
	}
	*/
	inline IMatrix<ww, hh> operator!()  const {
		IMatrix<ww, hh> dst;
		//transpose(dst.x, x, hh, ww);
		register int  i, j;
		for(i = 0; i < hh; i++) for(j = 0; j < ww; j++) {
			dst.x[j*hh + i] = x[i*ww + j];
		}
		return dst;
	}




	void makeEye() {
		make_eye(x, hh, ww);
	}

	/**    matrix trace 
		@return sum of the diagonal elements		
 */		
	XMType trace() const {
		return IMatrixImpl::trace(x, hh, ww);
	}

	/** @return  maximum element

	*/
	XMType max() const {
		int i;
		XMType ret = x[0];
		for(i = 1; i < hh*ww; i++) {
			if(ret < x[i]) {
				ret = x[i];
			}
		}
		return ret;
	}

	/**   For quick calculation of something like (H*H')
		@return dst = this * (!this)

	*/
	IMatrix<hh, hh> sp0() const {
		IMatrix<hh, hh>   dst;
		iiType i, j, k;
		for (i = 0; i < hh; i++) 	for (j = 0; j < hh; j++) {
			dst.x[i*hh+j] = ZERO;
			for (k = 0; k < ww; k++) {
				dst.x[i*hh+j] += x[i*ww+k] * x[j*ww+k];
			}
		}
		return dst;
	}




	// ====================================================================================
	/** "XMType" value multiplications (version #1)  NOT TESTED  */
	IMatrix<hh, ww> operator*(XMType d) const  {
		IMatrix<hh, ww> dst;
		int  i;
		for (i = 0; i < ww*hh; i++)  {
			dst.x[i] = x[i] * d;
		}

		return dst;
	}

	// .......................................................

	// .......................................................
	/** "XMType" value multiplications  (version #3 - fastest)  NOT TESTED  */
	void dmul(XMType d) {
		register int i;
		for (i = 0; i < hh*ww; i++) {
			x[i] *= d;
		}
	}

	// .......................................................
	/**  division operator. (version #1 - fastest)   NOT TESTED */
	void ddiv(XMType d) {
		register int i;
		for (i = 0; i < hh*ww; i++) {
			x[i] /= d;
		}
	}

	void operator/=(XMType d) {
		register int i;
		for(i = 0; i < hh*ww; i++) {
			x[i] /= d;
		}
	}

	// .......................................................
	/**  division operator. (version #2)   NOT TESTED */
	IMatrix<hh, ww> operator/(XMType d) const {
		IMatrix<hh, ww> dst;
		int  i;
		for (i = 0; i < ww*hh; i++)  {
			dst.x[i] = x[i] / d;
		}
		return dst;
	}

	// ====================================================================================
	/**   NOT TESTED   
 matrix root: get upper-triangular matrix.
		matrix should be square.
		@return square root of a matrix (upper-triangular)
	*/
	IMatrix<hh, ww> utchol() const { 
		IMatrix<hh, ww> ret; 
		bool ok = IMatrixImpl::utchol(ret.x, x, hh, ww);
		if (ok) {
			return ret;  
		}	else  {
			mxassert(false, "utchol faild");
			//return eyex<hh, ww>(); 
			return ret;
		}
	}

	// .......................................................
	/**  NOT TESTED
	 matrix root: get lower-triangular matrix.
		matrix should be square.
		@return square root of a matrix (lower-triangular)
	*/
	IMatrix<hh, ww> ltchol() const { 
		IMatrix<hh, ww> ret; 
		bool ok = IMatrixImpl::ltchol(ret.x, x, hh, ww);
		if (ok) 
			return ret;  
		else 
			mxassert(false, "ltchol faild");
			return ret;
	}


	/**    NOT TESTED
QR decomposition algorithm (version #1  not so fast).  
	A = Q*R.
	Use this function if you need "Q" matrix in the result.
			@param[out] Q
			@param[out] R
			@return true if all is OK
	*/
	bool qr_big(IMatrix<hh, hh>& Q, IMatrix<hh, ww>& R) const {
		XMType Qj[hh*hh];
		XMType q1[hh*hh];
		XMType c[hh];
		XMType tau[hh];
		XMType u[hh];
		bool ok;

#ifdef USE_GLS_LIBRARY
		if (hh != ww) {
			ok = IMatrixImpl::qr_gsl(x, R.x, c, tau, hh, ww, Q.x, u, Qj, q1);
		} else { // square matrix:
			ok = IMatrixImpl::qr(x, R.x, c, tau, hh, Q.x, Qj, q1, u);
		}
#else
		mxassert(hh == ww, "only square matrix supported");
		ok = IMatrixImpl::qr(x, R.x, c, tau, hh, Q.x, Qj, q1, u);
#endif
		return ok;
	}

	/**   NOT TESTED
			decomposition algorithm.  (version #1  fast)
			A = Q*R.
			Use this function if you do not need "Q" matrix in the result.
			@return   matrix R
	*/
	IMatrix<hh, ww> qr_small() const {
		XMType c[hh];	// #
		XMType tau[hh]; //  #
		IMatrix<hh, ww> R;
		bool ok;
#ifdef USE_GLS_LIBRARY
		if (hh != ww) {
			ok = IMatrixImpl::qr_gsl(x, R.x, c, tau, hh, ww);
		} else  { // square matrix:
			 ok = IMatrixImpl::qr(x, R.x, c, tau, hh);
		}
#else
		mxassert(hh == ww, "IMatrix<hh, ww> qr() only square matrix supported");
		ok = IMatrixImpl::qr(x, R.x, c, tau, hh);
#endif
		return R;
	}

	/** make this matrix simmetric

	*/
	inline void makeSimmetric() {
		int d = (ww > hh) ? hh : ww;
		int i, j;
		for(i = 1; i < d; i++) {
			for(j = 0; j < i; j++) {
				x[i*ww + j] = x[j*ww + i];
			}
		}
	}

	/** very good and very efficient  matrix inversion. 
	no return value: "this" matrix is inverted.
	(from the internet, (c) Drobotenko    http://drobotenko.com )
	CPU: [s^3; s^3 + 2s^2 + s; 0]
	*/
	void invSelf() {
	 
		int xm_gaus_ostatok;
		// 0 == ��

		
#ifdef _DEBUG
		XMType xm_gaus_deter;
		XMType xm_gaus_minved;
#endif
	   int rn[ww], cn[ww];
	   int j, k;

	   for(j = ww; j--;)
		 rn[j] = cn[j] = j;
	   
#ifdef _DEBUG
	   xm_gaus_minved = BIGNUMBER;
	   xm_gaus_deter = ONE;
#endif
	   for (xm_gaus_ostatok = ww; xm_gaus_ostatok; xm_gaus_ostatok--)    { 
		  int jved=-1, kved=-1;
		  XMType vved = -ONE, t;

		  for(j = ww; j--; )       {  
			 if(~rn[j])
				 for(k = ww; k--;)
				   if(~cn[k])
					   if(vved<fabs(x[j*hh+k]))
						  vved=fabs(x[j*hh+k]), jved = j, kved = k;
		  }
	      
#ifdef _DEBUG
		  	 if(xm_gaus_minved > vved)
				xm_gaus_minved = vved;
		  xm_gaus_deter *= x[jved*hh+kved];
#endif
		  mxassert(vved>VERYSMALLNUMBER, "matrix2 invSelf faild #5234 ");
		  if(vved<VERYSMALLNUMBER) {
			  for(j=ww;j--;)  { 
				if(~rn[j]) for(k=ww;k--;)
					x[j*hh+k]=_xm_NAN;
				if(~cn[j]) for(k=ww;k--;)
					x[k*hh+j]=_xm_NAN;
			 }
			 return;
		  }
		  mxat(kved >= 0);
		  mxat(jved >= 0);

		  int jt = rn[jved], kt = cn[kved];

		  for(j=ww;j--;)
			  t=x[kt*hh+j],x[kt*hh+j]=x[jved*hh+j],x[jved*hh+j]=t;
		  for(j=ww;j--;)
			  t=x[j*hh+jt],x[j*hh+jt]=x[j*hh+kved],x[j*hh+kved]=t;

		  rn[jved] = rn[kt];
		  cn[kved] = cn[jt];
		  rn[kt] = cn[jt] = -1;

		  vved = x[kt*hh+jt];   x[kt*hh+jt] = ONE;
		  for(j = ww; j--;)   { 
			  if(j==kt)
			   continue;

			 XMType mul = x[j*hh+jt] / vved;
			 x[j*hh+jt] = ZERO;
			 for(k = ww; k--;)
			   x[j*hh+k] -= x[kt*hh+k] * mul;
		  }
		  for(k = ww; k--;)
			  x[kt*hh+k] /= vved;
	   }
			
	}

	// ==============================================================================
#ifdef WIN32
	/** 	fill matrix with random numbers (for testing etc).	*/
	void fillrnd() { IMatrixImpl::fillrnd(); 	}

	// ==============================================================================
	/**   read a matrix from text file  */
	void fromTextFile(char* fileName) {   IMatrixImpl::fromTextFile(fileName);    }
#endif

	// ==============================================================================
#ifdef _DEBUG 
	/**		print matrix on the screen  for debug.	*/
	void print(iiType p = 11) { IMatrixImpl::print(x, hh, ww, p); }
#endif

	// ==============================================================================
	/**  insert 1D matrix inside "this" .
		@param[in] m	matrix to insert 
		@param[in] from index from what element insert "m" into "this"
	*/
	template <iiType h, iiType  w> void insert1(const IMatrix<h, w>& m, 
			 iiType from) { 
		int size = h*w;
		memcpy(x + from, m.x, size*XMTSize);
	}

	void insert1(iiType from, const XMType* v, int size) {
		memcpy(x + from, v, size*XMTSize);
	}
	// ==============================================================================
	/**  insert 2D matrix inside "this".
		@param[in] m	matrix to insert 
		@param[in] fh index from what element insert "m" into "this" - row
		@param[in] fw index from what element insert "m" into "this" - column
	*/
	template <iiType h, iiType  w> void insert2(const IMatrix<h, w>& m, iiType fh, iiType  fw) { 
		iiType i;
		mxassert((hh >= h+fh) && (ww >= w+fw), "matrix2 insert2 index error ");
		for (i = 0; i < h; i++) {
			memcpy(x+(fh+i)*ww + fw, m.x + i*w, w*XMTSize);
		}
	}

protected:
	
	// ==============================================================================
	/**   initial init of the data members. 
			it \b should be called in every constructor
	*/
	inline void init(const char* _name) {	
#ifdef _DEBUG  
		IMatrixImpl::init(_name);
#else
		_name = _name;
#endif 
	//	IMatrixImpl::empty(x, hh, ww); // !
		empty();
	}
}; //   END OF MATRIX  CLASS

// ##############################################################################

typedef	IMatrix<3, 1> Vector3;	///< 3 - element  vector
typedef	IMatrix<3, 1> V3;	///< 3 - element  vector
typedef	IMatrix<6, 1> Vector6;	///< 6 - element vector
typedef IMatrix<3, 3> M33;		///< 3x3  matrix
typedef IMatrix<3, 3> DCM;		///< 3x3 orientation matrix. Same as M33 now.

// ##############################################################################
// ##############################################################################
// ##############################################################################
// ##############################################################################


/** convert euler angles into DCM matrix.

	 @param[in] eu  orientation in  euler angles, [radians]
		 eu(0) = YAW; eu(1) = PITCH; eu(2) = ROLL;
		 rotation sequence is   Z->Y->X

		 @return direction cosine matrix
*/
DCM euler2dcm(const Vector3& eu);

/** create a rotation matrix from two sequential rotations.
	first rotation - around Y axis
	second rotation - around Z axis
	@param[in] b Y axis rotation [radians] 
	@param[in] g Z axis rotation [radians] 
	@return rotation matrix
*/
DCM euler2dcm_YZ(XMType b, XMType g);

/**
    vector cross product.
    CPU: [3; 6; 0]
    @param[in]  m1    first vector.
    @param[in]  m2	second vector.
    @return v_ret = m1  x  m2
*/
Vector3	cross(const Vector3& m1, const Vector3& m2);

/**  NOT TESTED 
	Create skew-symmetric matrix from a vector.
	\code
	[ x  y  z]    -> 

	 0	  z		-y
	-z	  0		 x
	 y	 -x		 0
	 \endcode

*/
M33	crossform(const Vector3& v);

/** NOT TESTED 
	Create skew-symmetric matrix from a vector.
	\code
	[ x  y  z]    -> 

	 0	  -z		y
	 z	   0		-x
	 -y	   x		0
	 \endcode
*/

M33	crossform2(const Vector3& v);

/**
	 extruct euler angles from rotation matrix.
	 @param[in] dcm    rotation matrix
	 \b result:
	\code
	[0] yaw
	[1]	pitch
	[2]	roll
	\endcode
	Rotation order should be checked (Z->Y->X ?)
*/
Vector3	dcm2euler(DCM& dcm);


/** NOT TESTED
	make non-square eye matrix.
*/
template < iiType h, iiType  w> IMatrix<h, w>	eye() {
	IMatrix<h, w> m;
	IMatrixImpl::make_eye(m.x, h, w);
	return m;
}

/**  calculate 'ext product' for a column vector, ret = x * x'
 (used in UKF)
*/
template <int hh> IMatrix<hh, hh> esp(const IMatrix<hh, 1>& m) {
	IMatrix<hh, hh> ret; 
	int i, j;
	for(i = 0; i < hh; i++) for(j = 0; j < hh; j++) {
		ret(i, j) = m[i] * m[j];
	}
	return ret;
}

/**  calculate 'ext product' for two column vectors, ret = x1 * x2'
(used in UKF)
*/
template <int h1, int h2> IMatrix<h1, h2> esp(const IMatrix<h1, 1>& x1, const IMatrix<h2, 1>& x2) {
	IMatrix<h1, h2> ret;
	int i, j;
	for(i = 0; i < h1; i++) for(j = 0; j < h2; j++) {
		ret(i, j) = x1[i] * x2[j];
	}
	return ret;
}

extern const DCM	eye33;

template <int hh, int ww> class IMView {
public:
	template<iiType s1, iiType s2> void mvInit(IMatrix<s1, s2>& m, int h1, int w1) {
		mxassert((hh + h1 <= s1) && (ww + w1 <= s2), "");
		hm = s1; wm = s2;
		h0 = h1;   w0 = w1;
		x = m.x;
	}

	/**
		@return copy of this matrix
	*/
	operator IMatrix<hh, ww>() const {
		IMatrix<hh, ww> ret;
		iiType r2 = h0 + hh - 1;
		// mxassert(((r2 < hm) && (w0 + ww - 1 < wm)), "");

		for(int i = h0; i <= r2; i++) {
			memcpy(ret.x + (i - h0)*ww, x + i*wm + w0, ww*XMTSize);
		}

		return ret;
	}

	/** make everything zero.

	*/
	void empty() {
		for(int i = h0; i < h0 + hh; i++) {
			memset(x + (h0 + i)*wm + w0, 0, ww*XMTSize);
		}
	}

	/**
		@return this matrix, multiplied by some number
	*/
	IMatrix<hh, ww> operator*(XMType d) const {
		int i, j;
		IMatrix<hh, ww> m;
		iiType r2 = h0 + hh - 1;
		// mxassert(((r2 < hm) && (w0 + ww - 1 < wm)), "");

		for(i = h0; i <= r2; i++) {
			memcpy(m.x + (i - h0)*ww, x + i*wm + w0, ww*XMTSize);
		}
		for(i = 0; i < hh*ww; i++) {
			m.x[i] *= d;
		}

		return m;
	}


	/** copy other matrix into 'this'

	*/
	void operator=(const IMatrix<hh, ww>& m) {
		//mxassert((hm >= hh + h0) && (wm >= ww + w0), ""); we checked this already
		for(int i = 0; i < hh; i++) {
			memcpy(x + (h0 + i)*wm + w0, m.x + i*ww, ww*XMTSize);
		}

	}

	/** add a matrix.

	*/
	inline void operator+=(const IMatrix<hh, ww>& m) {
		register int i, j;
		for(i = 0; i < hh; i++) for(j = 0; j < ww; j++) {
			x[(h0 + i)*wm + w0 + j] += m.x[i*ww + j];
		}
	}


	inline void operator-=(const IMatrix<hh, ww>& m) {
		register int i, j;
		for(i = 0; i < hh; i++) for(j = 0; j < ww; j++) {
			x[(h0 + i)*wm + w0 + j] -= m.x[i*ww + j];
		}
	}

	/** return a reference to some element

	*/
	XMType& operator()(int i, int j) {
		mxat((i+h0 < hm) && (j + w0 < wm));
		return x[(h0+i)*wm + w0 + j];
	}

	/** Do we need this really? I do not know.

	*/
	XMType operator()(int i, int j) const{
		mxat((i + h0 < hm) && (j + w0 < wm));
		return x[(h0 + i)*wm + w0 + j];
	}


private:
	int h0, w0; // our position inside matrix
	int hm, wm; //  matrix dimension
	XMType* x;
};

typedef IMView<3, 3> V33;		///< 3x3  matrix view

/** looks like this is not used.

*/
template <int hh> class IVector: public IMatrix < hh, 1 > {
public:
	IVector() {
		memset(this->x, 0, hh*XMTSize);
	}
	IVector(IMatrix <hh, 1> m) {
		memcpy(this->x, m.x, hh*XMTSize);
	}
};


//  ########################################################
//  ########################################################
//  ########################################################
//  ########################################################

/** \class Qua
	Quaternion class.

*/
class Qua : public IMatrix<4, 1> {
public:

	// ==============================================================================
	/**		INIT  empty quaternion.
	*/
	Qua (const char* _name = 0);

	// ==============================================================================
	/**		INIT   quaternion  from matrix  4x1.	*/
	Qua (const IMatrix<4, 1>& m, const char* _name = 0);

	// ==============================================================================
	/**  init quaternion from rotation vector. 	*/
	Qua (const Vector3& v,  const char* _name = 0);

	Qua(const XMType v[4]) {
		init(0);
		memcpy(x, v, 4*XMTSize);
	}

/*
	operator IMatrix<4, 1>&()  {
		return *this; 
	}
	*/
	// ==============================================================================
#ifdef MATLAB_MEX_FILE
	/**  init quaternion from Matlab var. 	*/
	Qua(const mxArray* mx, const char* _name = 0) : IMatrix<4, 1>(mx, _name) { }
#endif

	// ==============================================================================
	/**  init quaternion from rotation vector. 
		CPU: [3;  9;  3]
	*/
	inline void from_rv(const Vector3& v)   {
		XMType df = v.norma2();
		if (df <= DELTA_ZERO_1) {
			x[0] = ONE;   x[1] = ZERO; x[2] = ZERO; x[3] = ZERO;
		}  else {
			df = sqrt(df);
			XMType tmp = sin(df * HALF) / df;
			x[0] = cos(df * HALF);
			x[1] = tmp*v.x[0];
			x[2] = tmp*v.x[1];
			x[3] = tmp*v.x[2];
		}
	}

	/** return rotation vector

	*/
	Vector3 to_rv() const {
		Vector3 ret;
		XMType df = TWO * acos(x[0]);
		XMType tmp = sin(df * HALF) / df;
		ret.x[0] = x[1] / tmp;
		ret.x[1] = x[2] / tmp;
		ret.x[2] = x[3] / tmp;
		return ret;
	}

	// ==============================================================================
	/**   init quaternion from DCM.  
	*/
	Qua(const DCM& m,   const char* _name = 0);

	// ==============================================================================
	/**	init quaternion from another one.
	*/
	Qua (const Qua& q, const char* _name = 0);

	void take0(XMType q0, XMType q1, XMType q2, XMType q3) {
		x[0] = q0; x[1] = q1;  x[2] = q2; x[3] = q3;
	}

	// ==============================================================================
	/**		init quaternion as part of a 1D matrix.
	*/
	template < iiType h2> Qua(const IMatrix<h2, 1>& m, 
				 iiType from, const char* _name = 0)
		:	IMatrix<4, 1>(m, from, _name)  		{	}

	Qua& operator=(const IMatrix<4, 1>& m);		///< Qua = matrix

	/*
	void ftake(const float* a) {
		for(int i = 0; i < 4; i++) {
			x[i] = a[i];
		}
	}
	



	void putf(float* a) const {
		for(int i = 0; i < 4; i++) {
			a[i] = x[i];
		}
	}
*/
	// ==============================================================================
	/**	 	make rotation matrix (DCM) from a quaternion.
	*/
	inline DCM dcm() const {
		DCM dcm1("dcm 1");
	//	Qua qq; memcpy(qq.x, x, sizeof(XMType) * 4); 
	//	qq.normalize();  // TODO: remove this sometimes

		dcm1(0, 0) = ONE - TWO * (x[2] * x[2] + x[3] * x[3]);
		dcm1(0, 1) = TWO*(x[1]*x[2] + x[0]*x[3]);
		dcm1(0, 2) = TWO*(x[1]*x[3] - x[0]*x[2]);

		dcm1(1, 0) = TWO*(x[1]*x[2] - x[0]*x[3]);
		dcm1(1, 1) = ONE - TWO * (x[1] * x[1] + x[3] * x[3]);
		dcm1(1, 2) = TWO*(x[2]*x[3] + x[0]*x[1]);

		dcm1(2, 0) = TWO*(x[1]*x[3] + x[0]*x[2]);
		dcm1(2, 1) = TWO*(x[2]*x[3] - x[0]*x[1]);
		dcm1(2, 2) = ONE - TWO * (x[1] * x[1] + x[2] * x[2]);
		
	/*dcm1(0, 0) = x[0]*x[0] + x[1]*x[1] - x[2]*x[2] - x[3]*x[3];
		dcm1(0, 1) = TWO*(x[1]*x[2] - x[0]*x[3]);
		dcm1(0, 2) = TWO*(x[1]*x[3] + x[0]*x[2]);
		dcm1(1, 0) = TWO*(x[1]*x[2] + x[0]*x[3]);
		dcm1(1, 1) = x[0]*x[0] - x[1]*x[1] + x[2]*x[2] - x[3]*x[3];
		dcm1(1, 2) = TWO*(x[2]*x[3] - x[0]*x[1]);
		dcm1(2, 0) = TWO*(x[1]*x[3] - x[0]*x[2]);
		dcm1(2, 1) = TWO*(x[2]*x[3] + x[0]*x[1]);
		dcm1(2, 2) = x[0]*x[0] - x[1]*x[1] - x[2]*x[2] + x[3]*x[3];
*/
		return dcm1;
	}

	// ==============================================================================
	/**	get euler angles  from quaternion.
	rotation order:  Z -> Y -> X
	\code
	[0] yaw
	[1]	pitch
	[2]	roll
	\endcode
	FIXIT: this function is not correct
	*/
	Vector3 euler() const;

	// ==============================================================================
	/**  normalize quaternion (version #1 fast). 
		CPU: [7; 10; 0]
	*/
	inline void norm_fast() {
		XMType dq = (ONE - norma2());
		//if ((delta*delta) > DELTA_ZERO_1) {
		if (fabs(dq) > DELTA_ZERO_1) {
			XMType tmp = ONE + HALF*dq;
			x[0] = x[0] * tmp;
			x[1] = x[1] * tmp;
			x[2] = x[2] * tmp;
			x[3] = x[3] * tmp;
		}
	}

	// ==============================================================================
	/**	 multiply 2 quaternions.
		CPU: [12; 16; 0]
		@param[in] q second quaternion
	*/
	inline Qua qprod(const Qua& q)  const  {
		Qua ret;
		ret(0) = x[0]*q.x[0] - x[1]*q.x[1] - x[2]*q.x[2] - x[3]*q.x[3];
		ret(1) = x[0]*q.x[1] + x[1]*q.x[0] + x[2]*q.x[3] - x[3]*q.x[2];
		ret(2) = x[0]*q.x[2] - x[1]*q.x[3] + x[2]*q.x[0] + x[3]*q.x[1];
		ret(3) = x[0]*q.x[3] + x[1]*q.x[2] - x[2]*q.x[1] + x[3]*q.x[0];

		return ret;
	}

	Qua& operator*=(const Qua& q) {
		XMType ret[4];
		ret[0] = x[0] * q.x[0] - x[1] * q.x[1] - x[2] * q.x[2] - x[3] * q.x[3];
		ret[1] = x[0] * q.x[1] + x[1] * q.x[0] + x[2] * q.x[3] - x[3] * q.x[2];
		ret[2] = x[0] * q.x[2] - x[1] * q.x[3] + x[2] * q.x[0] + x[3] * q.x[1];
		ret[3] = x[0] * q.x[3] + x[1] * q.x[2] - x[2] * q.x[1] + x[3] * q.x[0];
		memcpy(x, ret, 4 * XMTSize);
		return *this;
	}

	// ==============================================================================
	/**	 NOT TESTED
	  quaternion conjugate.*/
	inline Qua	conj() const{
		Qua tmp;
		tmp.x[0] = x[0];
		tmp.x[1] = -x[1];
		tmp.x[2] = -x[2];
		tmp.x[3] = -x[3];
		return tmp;
	}

	// ==============================================================================
	/** NOT TESTED  set new YAW value on quaternion - NOT WORKING???? */
	void setYaw(XMType yaw);

	// ==============================================================================
	/** NOT TESTED  get YAW value  */
	XMType yaw();

	// ==============================================================================

}; //  END OF QUATERNION CLASS

	/**	  convert euler angles to quaternion.
	Rotation order: Z -> Y -> X.
	\code
		eu[0]  yaw
		eu[1]  pitch  
		eu[2]  roll    
	\endcode
	*/

Qua euler2qua(const Vector3& eu);
Qua dcm2qua(const DCM& m);

/**   NOT TESTED 
	Quaternion interpolation.
	p21 should be  between 0 and 1
	if p12 == 0,  return q1;
	if p12 == 1,  return q2;

	@param[in] p12   interpolation parameter.
	This method was invented by Nikolay Vasiluk 16.05.2007

*/
inline Qua quainterp(const Qua& q1, const Qua& q2x, XMType p12) {
	Qua ret;
		
	if (p12 < ZERO) p12 = 0;
	if (p12 > ONE) p12 = 1;

	// 0. check sign:
	Qua q2 = q2x;
	if ((q1(0) * q2(0)) < 0) { q2 = q2 * (-ONE); }

	//  1. rotation from q1 into q2:     "q2 = dq*q1"
	Qua dq = q2.qprod(q1.conj());

	//  one more check:
	if (dq(0) >= ONE) {
		ret = q1;
		return ret;
	}

	//   angle of this rotation:
	XMType f = 2.0 * acos(dq(0));  
	ret.x[0] = cos(p12 * f / 2.0);
	//  axis of this rotation:
	XMType k = sqrt((ONE - ret.x[0] * ret.x[0]) / (dq.x[1] * dq.x[1] + dq.x[2] * dq.x[2] + dq.x[3] * dq.x[3]));
	ret.x[1] = dq.x[1] * k; ret.x[2] = dq.x[2] * k; ret.x[3] = dq.x[3] * k;
	//   rotation:
	ret = ret.qprod(q1);

	return ret;
}

/** SLERP interpolation from 510's code.    NOT TESTED

*/
void slerp510(const Qua& from, const Qua& to, XMType t, Qua& res);

#include "eigsolver.h"
/**
  creates a weighted mean for a set of quaternions
  http://barnesc.blogspot.ru/2007/02/eigenvectors-of-3x3-symmetric-matrix.html

*/
template<int nsp> Qua qMean(IMatrix<4, nsp>& qi, IMatrix<nsp, 1> w) {
	IMatrix<4, 4> M;  M.empty();
	IMatrix<4, 1> q;
	IMatrix<4, 4> tmp;
	int i;
	for(i = 0; i < nsp; i++) {
		q.take2(qi, 0, i);
		tmp = q.sp0();
		tmp.dmul(w[i]);
		M += tmp;
	}
	double A[4][4];
	memcpy(A, M.x, 4 * 4 * 8);
	double V[4][4]; double d[4];
	eigen_decomposition(A, V, d);

	double maxD = d[0];
	int k = 0;
	for(i = 1; i < 4; i++) {
		if(d[i] > maxD) {
			k = i;
			maxD = d[i];
		}
	}

	Qua ret;
	for(i = 0; i < 4; i++) {
		ret[i] = V[i][k];
	}
	return ret;
}

/** we have two different vectors: v1 and v2; we know thier projection onto two coordinate system: 'i' and 'e';
what is the rotation quaternion from frame 'e' into frame 'i'? 
	@return answer on the question above
*/
Qua rotationFromTwoVectors(const V3& v1i, const V3& v1e, const V3& v2i, const V3& v2e);

// ############################################################################
// ############################################################################
// ############################################################################
// ############################################################################

int roundX(XMType x);
long long roundXL(long double x);

/**	get gravitation from latitude and height.
	@param[in] latitude   latitude [rad]
	@param[in]    he   heigth [m]
	@return gravity  in [meter/sec^2]
	*/
XMType normalgravity(XMType latitude, XMType he);

/**
	CPU: [6; 18; 0]
	@param dt   distance between points
	@param y1    y-coord of 1 point
	@param y2    y-coord of 2 point
	@param y3    y-coord of 3 point
	@return under parabola area (between points #2 and #3)
	*/
XMType paraint(XMType dt, XMType  y1, XMType  y2, XMType  y3);

/**     least square    linear approximation.
	@param[in] x		x coords
	@param[in] y		y coords
	@param[in] N		number of points in x and y
	@param[out] ab		result: XMType[2] "a" and "b" params  of the line;
	*/
void 	linear_appr(XMType*	x, XMType*	y, int N, XMType* ab);

/** simplest linear interpolation between two points.

*/
XMType linInterp(long long x1, long long  x2, long long  xm, XMType y1, XMType y2);
XMType linInterp(XMType	x1, XMType x2, XMType xm, XMType y1, XMType y2);

/**
	@param[in] t if "0.", return v1;  if "1.", return v2;  if 0.5, return mean value
*/
Vector3 linInterp(const Vector3& v1, const Vector3& v2, double t);

/**     least square    parabolic approximation.
	@param[in] x		x coords
	@param[in] y		y coords
	@param[in] N		number of points in x and y
	@param[out] abc		result: XMType[3] "a" and "b" and "c"  params  of the parabola;
	*/

void     parabola_appr(XMType*	x, XMType*	y, int N, XMType*	abc);

/**
	@param[out] array log-distributed numbers.
	@return logspace distribution.

	*/
void logSpace(XMType *array, int size, XMType xmin, XMType xmax);

/**

	@return
	*/
XMType rand1();
XMType rand2();

/**	 http://www.taygeta.com/random/gaussian.html
	Gaussian distribution with zero mean and a standard deviation of one.
	@return
	*/
XMType randgauss();

/**
	@param sigma sigma
	*/
XMType randgauss(XMType sigma);

#ifdef PC__VERSION
#include <map>  //  for DMatrixPool

/** an information in an array form.
*/
class DMatrix {
public:
	DMatrix();
	~DMatrix();
	/** call it before everything else.  It will allocate memory.

	*/
	void init(int h_, int w_);
	void clear();
	inline bool ok() const {
		return ((x != 0) && (hh != 0) && (ww != 0));
	}
	inline double& operator()(const int i, const int j) {
		TLL(x == 0, "DMatrix");
		TLL((i >= hh) || (j >= ww), "bounds error in DMatrix::operator() #1 \n");
		return x[i*ww + j]; // in Release  no bounds check!!
	}

	inline double operator()(const int i, const int j) const {
		TLL(x == 0, "DMatrix");
		TLL((i >= hh) || (j >= ww), "bounds error in DMatrix::operator() #2 \n");
		return x[i*ww + j]; // in Release  no bounds check!!
	}

	/** 
	@param i column number
	@return a column from this matrix.
	*/
	template<int hhh> IMatrix<hhh, 1> get(long long i) const {
		TLL(x == 0, "DMatrix");
		TLL((i >= ww) || (hh != hhh), "bounds error in DMatrix::v() #1 \n");
		IMatrix<hhh, 1> m;
		for (int k = 0; k < hhh; k++) {
			m.x[k] = x[i + k*ww];
		}
		return m;
	}

	/** insert a column into a matrix.

	*/
	template<int hhh> void ins(const IMatrix<hhh, 1>& m, int i) {
		TLL(x == 0, "DMatrix");
		TLL((i >= ww) || (hh != hhh), "bounds error in DMatrix::ins() #2 \n");
		for (int k = 0; k < hhh; k++) {
			x[i + k*ww] = m.x[k];
		}
	}
	operator double*()  {
		return x;
	}

	DMatrix& operator=(const DMatrix& m) {
		if (this == &m) return *this;
		mxassert(m.ok(), "");
		if (x == 0) {
			init(m.hh, m.ww);
		} else {
			mxassert((hh == m.hh) && (ww == m.ww), "");
		}
		memcpy(x, m.x, hh*ww*sizeof(double));
		return *this;
	}
	/** 
	@param i row number
	@return pointer on a row 'i'
	*/
	double* r(int i) const {
		mxassert((x != 0) && (i < hh), "");
		return x + i*ww;
	}

	double*	x;
	int		hh, ww;
};

class DMatrixPool {
protected:
    std::map<DMatrix*, DMatrix*> dMatrixMap;
	void addMatrix(DMatrix* m, int h, int w);
	void clearMatrices();
};


#endif // linux and WIN32

/**    
*/
inline XMType sqr(XMType x) {
	return x*x;
}

// ############################################################################
// ############################################################################
// ############################################################################
// ############################################################################

#ifdef MATLAB_MEX_FILE

/**
		a variable to be saved in Matlab workspace.
		\class MWSVar
*/
class MWSVar {
public:
	iiType  count;
	MWSVar(iiType hh, iiType ww, const char* name1);
	MWSVar(iiType hh, iiType ww, const char* prefix, const char* name1, iiType number);
	MWSVar() : mptr(0), count(0), v(0) {}

	/** init matlab var. 
	@param[in] hh variable dimension
	@param[in] ww variable dimension
	@param[in] name1 variable  name
	*/
	void init(iiType hh, iiType ww, const char* name1);

	/** init matlab var. 
	@param[in] hh variable dimension
	@param[in] ww variable dimension
	@param[in] name1 variable  name
	@param[in] prefix variable  name   prefix
	@param[in] number variable  name   suffix
	*/
	void init(iiType hh, iiType ww, const char* prefix, const char* name1, iiType number);

	~MWSVar();

	XMType& operator()(iiType i, iiType j);

	/**  save a variable in workspace .
	*/
	void save();

	/**  add    vector values to the variable.
	*/
	void add(const Vector3& a);

	/** prolong variable values.
	*/
	void stay();

	inline iiType getCount() { return count; }
	inline void  setCount(iiType c) {count = c; }
	void clear();

private:
	char		name[64]; ///<   name of a variable
	mxArray*	mptr;
	XMType*		v;
	iiType h, w;
	static XMType errorValue;
	

};

#endif


#endif



