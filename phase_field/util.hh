/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PHASE_FIELD__UTIL__
#define __PHASE_FIELD__UTIL__

#include "impl/state.hh"
#include "impl/type.hh"

namespace phase_field {
/**
 * @brief Set the nuclear to the 2D phase field
 *
 * @param r radius of the nuclear in grid size
 * @param f field 2D tensor
 */
inline void set_nuclear_to_corner(Field2D &f, const int64_t r) {
  const auto &f_shape = f.shape();
  const auto r_sq = static_cast<Field2D::scalar_type>(r * r);
  const int64_t xc = 0;
  const int64_t yc = 0;
  if (static_cast<int64_t>(f_shape[0]) <= r || static_cast<int64_t>(f_shape[1]) <= r) {
    throw std::invalid_argument("diameter too large");
  }

  for (int64_t y = 0; y < static_cast<int64_t>(f_shape[0]); ++y) {
    for (int64_t x = 0; x < static_cast<int64_t>(f_shape[1]); ++x) {
      const auto dx = static_cast<Field2D::scalar_type>(x - xc);
      const auto dy = static_cast<Field2D::scalar_type>(y - yc);
      if ((dx * dx + dy * dy) <= r_sq) {
        f[y][x] = FieldState::solid;
      } else {
        f[y][x] = FieldState::liquid;
      }
    }
  }
}
} // namespace phase_field

#endif