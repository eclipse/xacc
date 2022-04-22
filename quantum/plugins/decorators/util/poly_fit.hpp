
// ********************************************************************
// * Code PolyFit                                                     *
// * Written by Ianik Plante                                          *
// *                                                                  *
// * KBR                                                              *
// * 2400 NASA Parkway, Houston, TX 77058                             *
// * Ianik.Plante-1@nasa.gov                                          *
// *                                                                  *
// * This code is used to fit a series of n points with a polynomial  *
// * of degree k, and calculation of error bars on the coefficients.  *
// * If error is provided on the y values, it is possible to use a    *
// * weighted fit as an option. Another option provided is to fix the *
// * intercept value, i.e. the first parameter.                       *
// *                                                                  *
// * This code has been written partially using data from publicly    *
// * available sources.                                               *
// *                                                                  *
// * The code works to the best of the author's knowledge, but some   *
// * bugs may be present. This code is provided as-is, with no        *
// * warranty of any kind. By using this code you agree that the      *
// * author, the company KBR or NASA are not responsible for possible *
// * problems related to the usage of this code.                      *
// *                                                                  *
// * The program has been reviewed and approved by export control for *
// * public release. However some export restriction exists. Please   *
// * respect applicable laws.                                         *
// *                                                                  *
// ********************************************************************
#pragma once

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>
#include <math.h>
#include <cmath>
#include <cassert>
#include <iomanip>

namespace xacc {
std::vector<std::vector<double>> Make2DArray(const size_t rows,
                                             const size_t cols) {
  std::vector<std::vector<double>> array(rows);
  for (size_t i = 0; i < rows; i++) {
    array[i] = std::vector<double>(cols);
  }

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      array[i][j] = 0.;
    }
  }

  return array;
}

// Transpose a 2D array
// **************************************************************
std::vector<std::vector<double>>
MatTrans(const std::vector<std::vector<double>> &array) {
  const size_t rows = array.size();
  const size_t cols = array[0].size();
  auto arrayT = Make2DArray(cols, rows);

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      arrayT[j][i] = array[i][j];
    }
  }

  return arrayT;
}

// Perform the multiplication of matrix A[m1,m2] by B[m2,m3]
// **************************************************************
std::vector<std::vector<double>>
MatMul(const std::vector<std::vector<double>> &A,
       const std::vector<std::vector<double>> &B) {
  const size_t m1 = A.size();
  const size_t m2 = A[0].size();
  assert(B.size() == m2);
  const size_t m3 = B[0].size();
  auto array = Make2DArray(m1, m3);

  for (size_t i = 0; i < m1; i++) {
    for (size_t j = 0; j < m3; j++) {
      array[i][j] = 0.;
      for (size_t m = 0; m < m2; m++) {
        array[i][j] += A[i][m] * B[m][j];
      }
    }
  }
  return array;
}

// Perform the multiplication of matrix A[m1,m2] by vector v[m2,1]
// **************************************************************
void MatVectMul(const std::vector<std::vector<double>> &A,
                const std::vector<double> &v, std::vector<double> &Av) {
  const size_t m1 = A.size();
  const size_t m2 = A[0].size();
  assert(v.size() == m2);

  for (size_t i = 0; i < m1; i++) {
    Av[i] = 0.;
    for (size_t j = 0; j < m2; j++) {
      Av[i] += A[i][j] * v[j];
    }
  }
}

// Calculates the determinant of a matrix
// **************************************************************
double determinant(const std::vector<std::vector<double>> &a, const size_t k) {
  double s = 1;
  double det = 0.;
  auto b = Make2DArray(k, k);
  size_t m;
  size_t n;

  if (k == 1)
    return (a[0][0]);

  for (size_t c = 0; c < k; c++) {

    m = 0;
    n = 0;

    for (size_t i = 0; i < k; i++) {

      for (size_t j = 0; j < k; j++) {

        b[i][j] = 0;

        if (i != 0 && j != c) {

          b[m][n] = a[i][j];
          if (n < (k - 2)) {
            n++;
          } else {
            n = 0;
            m++;
          }
        }
      }
    }

    det = det + s * (a[0][c] * determinant(b, k - 1));
    s = -1 * s;
  }

  return (det);
}

// Perform the
// **************************************************************
void transpose(const std::vector<std::vector<double>> &num,
               const std::vector<std::vector<double>> &fac,
               std::vector<std::vector<double>> &inverse, const size_t r) {
  auto b = Make2DArray(r, r);
  double deter;

  for (size_t i = 0; i < r; i++) {
    for (size_t j = 0; j < r; j++) {
      b[i][j] = fac[j][i];
    }
  }

  deter = determinant(num, r);

  for (size_t i = 0; i < r; i++) {
    for (size_t j = 0; j < r; j++) {
      inverse[i][j] = b[i][j] / deter;
    }
  }
}

// Calculates the cofactors
// **************************************************************
void cofactor(const std::vector<std::vector<double>> &num,
              std::vector<std::vector<double>> &inverse, const size_t f) {

  auto b = Make2DArray(f, f);
  auto fac = Make2DArray(f, f);

  size_t m;
  size_t n;

  for (size_t q = 0; q < f; q++) {

    for (size_t p = 0; p < f; p++) {

      m = 0;
      n = 0;

      for (size_t i = 0; i < f; i++) {

        for (size_t j = 0; j < f; j++) {

          if (i != q && j != p) {

            b[m][n] = num[i][j];

            if (n < (f - 2)) {
              n++;
            } else {
              n = 0;
              m++;
            }
          }
        }
      }
      fac[q][p] = pow(-1, q + p) * determinant(b, f - 1);
    }
  }

  transpose(num, fac, inverse, f);
}

// Perform the fit of data n data points (x,y) with a polynomial of order k
// **************************************************************
void PolyFit(const std::vector<double> &x, const std::vector<double> &y,
             const size_t n, const size_t k, const bool fixedinter,
             const double fixedinterval, std::vector<double> &beta,
             const std::vector<std::vector<double>> &Weights,
             std::vector<std::vector<double>> &XTWXInv) {

  // Definition of variables
  // **************************************************************
  auto X = Make2DArray(n, k + 1); // [n,k+1]

  std::vector<double> XTWY(k + 1);
  std::vector<double> Y(n);

  size_t begin = 0;
  if (fixedinter)
    begin = 1;

  // Initialize X
  // **************************************************************
  for (size_t i = 0; i < n; i++) {
    for (size_t j = begin; j < (k + 1); j++) { // begin
      X[i][j] = pow(x[i], j);
    }
  }

  // Matrix calculations
  // **************************************************************
  auto XT = MatTrans(X);          // Calculate XT
  auto XTW = MatMul(XT, Weights); // Calculate XT*W
  auto XTWX = MatMul(XTW, X);     // Calculate (XTW)*X

  if (fixedinter) {
    XTWX[0][0] = 1.;
  }

  cofactor(XTWX, XTWXInv, k + 1); // Calculate (XTWX)^-1

  for (size_t m = 0; m < n; m++) {
    if (fixedinter) {
      Y[m] = y[m] - fixedinterval;
    } else {
      Y[m] = y[m];
    }
  }
  MatVectMul(XTW, Y, XTWY); // Calculate (XTW)*Y
  MatVectMul(XTWXInv, XTWY,
             beta); // Calculate beta = (XTWXInv)*XTWY

  if (fixedinter) {
    beta[0] = fixedinterval;
  }
}

std::vector<double> poly_fit(const std::vector<double> &x,
                             const std::vector<double> &y) {
  const size_t k = x.size() - 1;     // Polynomial order
  constexpr bool fixedinter = false; // Fixed the intercept (coefficient A0)
  constexpr double fixedinterval = 0.0;
  constexpr double alphaval = 0.05; // Critical apha value

  // Definition of other variables
  // **************************************************************
  const size_t n = x.size();           // Number of data points (adjusted later)
  std::vector<double> coefbeta(k + 1); // Coefficients of the polynomial
  const size_t nstar = n - 1;
  auto XTWXInv = Make2DArray(k + 1, k + 1);
  auto Weights = Make2DArray(n, n);
  for (size_t i = 0; i < n; ++i) {
    Weights[i][i] = 1.0;
  }
  // Calculate the coefficients of the fit
  // **************************************************************
  PolyFit(x, y, n, k, fixedinter, fixedinterval, coefbeta, Weights, XTWXInv);
  return coefbeta;
}
} // namespace xacc