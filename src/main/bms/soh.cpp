#include "Arduino.h"
#include "soh.h"
#include "Linear_Algebra.h"
#include <vector>

/* ------------------------------------------------------------------------------------------------
  DEFINES
------------------------------------------------------------------------------------------------ */
// #define DEBUG_SOH
/* ------------------------------------------------------------------------------------------------
  CONSTANTS
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
  EXTERNAL VARIABLES
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
  LOCAL VARIABLES
------------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------------
  FUNCTION PROTOTYPES
------------------------------------------------------------------------------------------------ */

/* For least squares, we are finding phi, where phi = [OCV; R_internal] 
  Phi = (A_transpose*A)^-1 * A_t * B
  A is a Nx2 matrix, first column all 1s, second column being the Nth current measurement
  B is a Nx1 matrix, with each row being the Nth voltage measurement */
soh_result soh_LeastSquares(const std::vector<float32>& current_mA_vf32, const std::vector<float32>& voltage_mV_vf32) {
  Matrix A(current_mA_vf32.size(), 2);
  /* Fill 2nd column of A with current measurements, 1st column with 1's */
  for (size_t i = 0; i < current_mA_vf32.size(); i++) {
    A(i,0) = 1;
    A(i,1) = current_mA_vf32[i];
  }
  Matrix A_t = transpose(A);
  Vector B(voltage_mV_vf32.size());
  /* Fill vector with voltage measurements*/
  for (size_t i = 0; i < voltage_mV_vf32.size(); i++) {
    B(i) = voltage_mV_vf32[i];
  }

  /* This library does not have an inverse function, so need to do by hand. Luckily this is 2x2 */
  Matrix mult1 = A_t * A;
  /* Compute inverse*/
  Matrix A_inv = mult1;
  /* Compute determinant */
  float32 matrixDet = mult1(0,0)*mult1(1,1) - mult1(0,1)*mult1(1,0);
  /* GUARD: check A_inv_det is not zero */
  if (matrixDet == 0.0) {
    // throw -1;
  }
  matrixDet = 1/ matrixDet;
  A_inv(0,0) = matrixDet*mult1(1,1);
  A_inv(0,1) = -matrixDet*mult1(0,1);
  A_inv(1,0) = -matrixDet*mult1(1,0);
  A_inv(1,1) = matrixDet*mult1(0,0);

  Matrix leastSquaresResult = A_inv * A_t * B;
  
  soh_result retVal = {
    .OCV_f32 = (float32)leastSquaresResult(0,0),
    .internalResistance_f32 = (float32)leastSquaresResult(1,0)
  };
  #ifdef DEBUG_SOH
  Serial.println("A matrix: ");
  Serial.println(A);
  Serial.println("B matrix: ");
  Serial.println(B);
  Serial.println("Transpose matrix: ");
  Serial.println(A_t);
  Serial.println("Mult Matrix");
  Serial.println(mult1);
  Serial.println("Inv matrix: ");
  Serial.println(A_inv);
  Serial.println("Res matrix: ");
  Serial.println(leastSquaresResult);
  #endif

  return retVal;
}

soh_result soh_LeastSquares(const float32 current_mA_af32[], const float32 voltage_mV_af32[], uint16 numSamples_ui16) {
  Matrix A(numSamples_ui16, 2) ;
  /* Fill 2nd column of A with current measurements, 1st column with 1's */
  for (size_t i = 0; i < numSamples_ui16; i++) {
    A(i,0) = 1;
    A(i,1) = current_mA_af32[i];
  }
  Matrix A_t = transpose(A);
  Vector B(numSamples_ui16);
  /* Fill vector with voltage measurements*/
  for (size_t i = 0; i < numSamples_ui16; i++) {
    B(i) = voltage_mV_af32[i];
  }

  /* This library does not have an inverse function, so need to do by hand. Luckily this is 2x2 */
  Matrix mult1 = A_t * A;
  /* Compute inverse*/
  Matrix A_inv = mult1;
  /* Compute determinant */
  float32 matrixDet = mult1(0,0)*mult1(1,1) - mult1(0,1)*mult1(1,0);
  /* GUARD: check A_inv_det is not zero */
  if (matrixDet == 0.0) {
    // throw -1;
  }
  matrixDet = 1/matrixDet;
  A_inv(0,0) = matrixDet*mult1(1,1);
  A_inv(0,1) = -matrixDet*mult1(0,1);
  A_inv(1,0) = -matrixDet*mult1(1,0);
  A_inv(1,1) = matrixDet*mult1(0,0);

  Matrix leastSquaresResult = A_inv * A_t * B;
  
  soh_result retVal = {
    .OCV_f32 = (float32)leastSquaresResult(0,0),
    .internalResistance_f32 = (float32)leastSquaresResult(1,0)
  };
  #ifdef DEBUG_SOH
  Serial.println("A matrix: ");
  Serial.println(A);
  Serial.println("B matrix: ");
  Serial.println(B);
  Serial.println("Transpose matrix: ");
  Serial.println(A_t);
  Serial.println("Mult Matrix");
  Serial.println(mult1);
  Serial.println("Inv matrix: ");
  Serial.println(A_inv);
  Serial.println("Res matrix: ");
  Serial.println(leastSquaresResult);
  #endif

  return retVal;
}



