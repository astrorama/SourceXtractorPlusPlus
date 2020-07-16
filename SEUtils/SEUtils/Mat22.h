/*
 * Mat22.h
 *
 *  Created on: Jul 25, 2019
 *      Author: mschefer
 */

#ifndef _SEUTILS_MAT22_H_
#define _SEUTILS_MAT22_H_

#include <tuple>
#include <assert.h>

namespace SExtractor {

class Mat22;
Mat22 operator*(const Mat22& a, const Mat22& b);

class Mat22 {
public:

  Mat22() : m_mat { 1.0, 0.0, 0.0, 1.0 } {
  }

  Mat22(double m1, double m2, double m3, double m4) : m_mat { m1, m2, m3, m4 } {
  }

  Mat22(std::tuple<double, double, double, double> tuple) : m_mat {
    std::get<0>(tuple),
    std::get<1>(tuple),
    std::get<2>(tuple),
    std::get<3>(tuple)} {}

  Mat22(const Mat22&) = default;

  Mat22& operator=(const Mat22& other) {
    m_mat[0] = other[0];
    m_mat[1] = other[1];
    m_mat[2] = other[2];
    m_mat[3] = other[3];

    return *this;
  }

  double operator[](size_t i) const {
    return m_mat[i];
  }

  double& operator[](size_t i) {
    return m_mat[i];
  }

  Mat22& operator*=(const Mat22& other) {
    *this = *this * other;
    return *this;
  }

  Mat22 GetInverse() const {
    Mat22 out;

    double inv_det = 1.  / (m_mat[0] * m_mat[3] - m_mat[2] * m_mat[1]);
    out[0] = m_mat[3] * inv_det; out[1] = -m_mat[1] * inv_det;
    out[2] = -m_mat[2] * inv_det; out[3] = m_mat[0] * inv_det;

    assert(m_mat[0] * out[0] + m_mat[1] * out[2] >= 1 - 1e-8);
    assert(m_mat[0] * out[1] + m_mat[1] * out[3] <= 1e-8);
    assert(m_mat[2] * out[0] + m_mat[3] * out[2] <= 1e-8);
    assert(m_mat[2] * out[1] + m_mat[3] * out[3] >= 1 - 1e-8);

    return out;
  }

  Mat22 GetTranspose() const {
    Mat22 out { m_mat[0], m_mat[2],
                m_mat[1], m_mat[3] };

    return out;
  }

  double m_mat[4];
};

inline Mat22 operator*(const Mat22& a, const Mat22& b) {
  Mat22 out;

  out[0] = a[0] * b[0] + a[1] * b[2];
  out[1] = a[0] * b[1] + a[1] * b[3];
  out[2] = a[2] * b[0] + a[3] * b[2];
  out[3] = a[2] * b[1] + a[3] * b[3];

  return out;
}

inline Mat22 operator*(const Mat22& a, double b) {
  Mat22 out;

  out[0] = a[0] * b;
  out[1] = a[1] * b;
  out[2] = a[2] * b;
  out[3] = a[3] * b;

  return out;
}

}


#endif /* _SEUTILS_MAT22_H_ */
