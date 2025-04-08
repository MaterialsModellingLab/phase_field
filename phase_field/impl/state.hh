/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PHASE_FIELD__STATE__
#define __PHASE_FIELD__STATE__

#include "type.hh"

namespace phase_field {
struct FieldState {
  static constexpr Field2D::scalar_type liquid = -1.0;
  static constexpr Field2D::scalar_type solid = 1.0;
};
} // namespace phase_field

#endif // __PHASE_FIELD__STATE__