#ifndef ARDUINO
#include <linalg/RowPivotLU.hpp>
#else
#include <include/linalg/RowPivotLU.hpp>
#endif

#include <cassert>

void RowPivotLU::compute(SquareMatrix &&matrix) {
    LU = std::move(matrix);
    P.resize(LU.rows());
    P.fill_identity();
    compute_factorization();
}

void RowPivotLU::compute(const SquareMatrix &matrix) {
    LU = matrix;
    P.resize(LU.rows());
    P.fill_identity();
    compute_factorization();
}

SquareMatrix &&RowPivotLU::steal_L() {
    assert(has_LU());
    state = NotFactored;
    valid_LU = false;
    for (size_t c = 0; c < LU.cols(); ++c) {
        // Elements above the diagonal are zero
        for (size_t r = 0; r < c; ++r)
            LU(r, c) = 0;
        // Diagonal elements are one
        LU(c, c) = 1;
        // Elements below the diagonal are stored in LU already
    }
    return std::move(LU);
}

void RowPivotLU::get_L_inplace(Matrix &L) const {
    assert(has_LU());
    assert(L.rows() == LU.rows());
    assert(L.cols() == LU.cols());
    for (size_t c = 0; c < L.cols(); ++c) {
        // Elements above the diagonal are zero
        for (size_t r = 0; r < c; ++r)
            L(r, c) = 0;
        // Diagonal elements are one
        L(c, c) = 1;
        // Elements below the diagonal are stored in LU
        for (size_t r = c + 1; r < L.rows(); ++r)
            L(r, c) = LU(r, c);
    }
}

SquareMatrix RowPivotLU::get_L() const & {
    SquareMatrix L(LU.rows());
    get_L_inplace(L);
    return L;
}

SquareMatrix &&RowPivotLU::steal_U() {
    assert(has_LU());
    state = NotFactored;
    valid_LU = false;
    for (size_t c = 0; c < LU.cols(); ++c) {
        // Elements above and on the diagonal are stored in LU already
        // Elements below the diagonal are zero
        for (size_t r = c + 1; r < LU.rows(); ++r)
            LU(r, c) = 0;
    }
    return std::move(LU);
}

void RowPivotLU::get_U_inplace(Matrix &U) const {
    assert(has_LU());
    assert(U.rows() == LU.rows());
    assert(U.cols() == LU.cols());
    for (size_t c = 0; c < U.cols(); ++c) {
        // Elements above and on the diagonal are stored in LU
        for (size_t r = 0; r <= c; ++r)
            U(r, c) = LU(r, c);
        // Elements below the diagonal are zero
        for (size_t r = c + 1; r < U.rows(); ++r)
            U(r, c) = 0;
    }
}

SquareMatrix RowPivotLU::get_U() const & {
    SquareMatrix U(LU.rows());
    get_U_inplace(U);
    return U;
}

PermutationMatrix &&RowPivotLU::steal_P() {
    state = NotFactored;
    valid_P = false;
    return std::move(P);
}

SquareMatrix &&RowPivotLU::steal_LU() {
    state = NotFactored;
    valid_LU = false;
    return std::move(LU);
}

Matrix RowPivotLU::solve(const Matrix &B) const {
    Matrix B_cpy = B;
    solve_inplace(B_cpy);
    return B_cpy;
}

Matrix &&RowPivotLU::solve(Matrix &&B) const {
    solve_inplace(B);
    return std::move(B);
}

Vector RowPivotLU::solve(const Vector &b) const {
    return Vector(solve(static_cast<const Matrix &>(b)));
}

Vector &&RowPivotLU::solve(Vector &&b) const {
    solve_inplace(b);
    return std::move(b);
}

#ifndef NO_IOSTREAM_SUPPORT

#include <iomanip>
#include <iostream>

// LCOV_EXCL_START

std::ostream &operator<<(std::ostream &os, const RowPivotLU &lu) {
    if (!lu.is_factored()) {
        os << "Not factored." << std::endl;
        return os;
    }

    // Output field width (characters)
    int w = os.precision() + 9;
    auto &LU = lu.get_LU();

    os << "L = " << std::endl;
    for (size_t r = 0; r < LU.cols(); ++r) {
        for (size_t c = 0; c < r; ++c)
            os << std::setw(w) << 0;
        for (size_t c = r; c < LU.cols(); ++c)
            os << std::setw(w) << LU(r, c);
        os << std::endl;
    }

    os << "U = " << std::endl;
    for (size_t r = 0; r < LU.cols(); ++r) {
        for (size_t c = 0; c < r; ++c)
            os << std::setw(w) << LU(r, c);
        os << std::setw(w) << 1;
        for (size_t c = r; c < LU.cols(); ++c)
            os << std::setw(w) << 0;
        os << std::endl;
    }
    return os;
}

// LCOV_EXCL_STOP

#endif
