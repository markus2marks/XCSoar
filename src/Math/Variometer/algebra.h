/*
  algebra.h: This file contains a number of utilities useful for handling
  3D vectors

  This work is an adaptation from vvector.h, written by Linas Vepstras. The
  original code can be found at:

  https://github.com/markkilgard/glut/blob/master/lib/gle/vvector.h

  HISTORY:
  Written by Linas Vepstas, August 1991
  Added 2D code, March 1993
  Added Outer products, C++ proofed, Linas Vepstas October 1993
  Adapted for altitude estimation tasks by Juan Gallostra June 2018
  Separated .h, .cpp by Simon D. Levy July 2018
*/

#pragma once

//#include <cmath>
#include <math.h>

// Copy 3D vector
void copyVector(double b[3],double a[3]);


// Vector difference
void subtractVectors(double v21[3], double v2[3], double v1[3]);

// Vector sum
void sumVectors(double v21[3], double v2[3], double v1[3]);

// scalar times vector
void scaleVector(double c[3],double a, double b[3]);

// accumulate scaled vector
void accumulateScaledVector(double c[3], double a, double b[3]);

// Vector dot product
void dotProductVectors(double * c, double a[3], double b[3]);

// Vector length
void vectorLength(double * len, double a[3]);

// Normalize vector
void normalizeVector(double a[3]);

// 3D Vector cross product yeilding vector
void crossProductVectors(double c[3], double a[3], double b[3]);

// initialize matrix
void identityMatrix3x3(double m[3][3]);

// matrix copy
void copyMatrix3x3(double b[3][3], double a[3][3]);

// matrix transpose
void transposeMatrix3x3(double b[3][3], double a[3][3]);

// multiply matrix by scalar
void scaleMatrix3x3(double b[3][3], double s, double a[3][3]);

// multiply matrix by scalar and add result to another matrix
void scaleAndAccumulateMatrix3x3(double b[3][3], double s, double a[3][3]);

// matrix product
// c[x][y] = a[x][0]*b[0][y]+a[x][1]*b[1][y]+a[x][2]*b[2][y]+a[x][3]*b[3][y]
void matrixProduct3x3(double c[3][3], double a[3][3], double b[3][3]);

// matrix times vector
void matrixDotVector3x3(double p[3], double m[3][3], double v[3]);

// determinant of matrix
// Computes determinant of matrix m, returning d
void determinant3x3(double * d, double m[3][3]);

// adjoint of matrix
// Computes adjoint of matrix m, returning a
// (Note that adjoint is just the transpose of the cofactor matrix);
void adjoint3x3(double a[3][3], double m[3][3]);

// compute adjoint of matrix and scale
// Computes adjoint of matrix m, scales it by s, returning a
void scaleAdjoint3x3(double a[3][3], double s, double m[3][3]);

// inverse of matrix
// Compute inverse of matrix a, returning determinant m and
// inverse b
void invert3x3(double b[3][3], double a[3][3]);

// skew matrix from vector
void skew(double a[3][3], double v[3]);

void printMatrix3X3(double mmm[3][3]);

void vecPrint(double a[3]);
