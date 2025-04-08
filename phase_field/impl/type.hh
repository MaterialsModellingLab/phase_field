/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PHASE_FIELD__TYPE__
#define __PHASE_FIELD__TYPE__

#include <libtensor/libtensor.hh>

namespace phase_field {
using Field2D = libtensor::Tensor<double, 2>;
using Field3D = libtensor::Tensor<double, 3>;

using Filter2D = libtensor::Tensor<double, 2>;
using Filter3D = libtensor::Tensor<double, 3>;
} // namespace phase_field

#endif // __PHASE_FIELD__TYPE__