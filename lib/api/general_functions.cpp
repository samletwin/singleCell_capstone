// #include "Arduino.h"
// #include "Linear_Algebra.h"

// /* ------------------------------------------------------------------------------------------------
//   DEFINES
// ------------------------------------------------------------------------------------------------ */

// /* ------------------------------------------------------------------------------------------------
//   CONSTANTS
// ------------------------------------------------------------------------------------------------ */

// /* ------------------------------------------------------------------------------------------------
//   EXTERNAL VARIABLES
// ------------------------------------------------------------------------------------------------ */

// /* ------------------------------------------------------------------------------------------------
//   LOCAL VARIABLES
// ------------------------------------------------------------------------------------------------ */

// /* ------------------------------------------------------------------------------------------------
//   FUNCTION PROTOTYPES
// ------------------------------------------------------------------------------------------------ */

// /// @brief Function to set an entire array to one constant value
// /// @tparam T the type of array (float, int, etc.)
// /// @param array_t reference of array
// /// @param arraySize_t size of array
// /// @param value_t value to set array to
// template <typename T> void api_SetArrayToValue(T* array_t, T arraySize_t, T value_t) {
//   for (size_t i = 0; i < arraySize_t; i++) {
//     array_t[i] = value_t;
//   }
// }

// /// @brief A function to compute the inverse of a 2x2 matrix
// /// @tparam T The data type of the matrix (i.e., float, int, etc.)
// /// @param inputMatrix The matrix to compute the inverse of
// /// @return The inverse of the input matrix
// template <typename T> Matrix api_ComputeInverse2x2(Matrix inputMatrix) {
//   Matrix copyMatrix = inputMatrix;
//   /* Compute determinant */
//   T matrixDet = copyMatrix(0,0)*copyMatrix(1,1) - copyMatrix(0,1)*copyMatrix(1,0);
//   /* GUARD: check A_inv_det is not zero */
//   if (matrixDet == 0) {
//     throw -1;
//   }
//   inputMatrix(0,0) = matrixDet*copyMatrix(1,1);
//   inputMatrix(0,1) = -matrixDet*copyMatrix(0,1);
//   inputMatrix(1,0) = -matrixDet*copyMatrix(1,0);
//   inputMatrix(1,1) = matrixDet*copyMatrix(0,0);

//   return inputMatrix;
// }

