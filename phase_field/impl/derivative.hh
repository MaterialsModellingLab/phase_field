/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PHASE_FIELD__DERIVATIVE__
#define __PHASE_FIELD__DERIVATIVE__

#include "type.hh"
#include <libtensor/filter.hh>

namespace phase_field {
inline const Filter2D get_dx_filter(const Field2D::scalar_type dx) {
  Filter2D f({3, 3});
  f[0][0] = 0.0, f[0][1] = 0.0, f[0][2] = 0.0;
  f[1][0] = -1.0, f[1][1] = 0.0, f[1][2] = 1.0;
  f[2][0] = 0.0, f[2][1] = 0.0, f[2][2] = 0.0;

  return f / (2.0 * dx);
}

inline const Filter2D get_dy_filter(const Field2D::scalar_type dx) {
  Filter2D f({3, 3});
  f[0][0] = 0.0, f[0][1] = -1.0, f[0][2] = 0.0;
  f[1][0] = 0.0, f[1][1] = 0.0, f[1][2] = 0.0;
  f[2][0] = 0.0, f[2][1] = 1.0, f[2][2] = 0.0;
  return f / (2.0 * dx);
}

inline const Filter2D get_laplacian_filter(const Filter2D::scalar_type dx) {
  Filter2D f({3, 3});
  f[0][0] = 0.0, f[0][1] = 1.0, f[0][2] = 0.0;
  f[1][0] = 1.0, f[1][1] = -4.0, f[1][2] = 1.0;
  f[2][0] = 0.0, f[2][1] = 1.0, f[2][2] = 0.0;
  return f / std::pow(dx, 2.0);
}

inline void conv2d(const Field2D &field, const Filter2D &filter, Field2D &ret) {
  libtensor::conv2d<Field2D::scalar_type>(field, filter, ret);
}
} // namespace phase_field

#endif // __PHASE_FIELD__DERIVATIVE__
