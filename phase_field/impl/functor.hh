/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PHASE_FIELD__FUNCTOR__
#define __PHASE_FIELD__FUNCTOR__

#include "state.hh"
#include "type.hh"
#include <algorithm>

namespace phase_field {
struct FieldClampFunctor {
  using T = Field2D::scalar_type;
  inline void operator()(T &ret) const {
    ret = std::clamp(ret, FieldState::liquid, FieldState::solid);
  }
};

struct InvAbsN4Functor {
  using T = Field2D::scalar_type;
  inline void operator()(T &ret, const T &dphi_dx, const T &dphi_dy) const {
    const T tmp = std::pow((std::pow(dphi_dx, 2.0) + std::pow(dphi_dy, 2.0)), 2.0);
    ret = tmp == 0.0 ? 0.0 : 1.0 / tmp;
  }
};

struct N4Functor {
  using T = Field2D::scalar_type;
  inline void operator()(T &ret, const T &dphi_dx, const T &dphi_dy, const T &inv_abs_n4) const {
    ret = (std::pow(dphi_dx, 4.0) + std::pow(dphi_dy, 4.0)) * inv_abs_n4;
  }
};

struct AcFunctor {
  using T = Field2D::scalar_type;
  const T c1, c2;
  AcFunctor(const T &epsilon_c) : c1(1.0 - 3.0 * epsilon_c), c2(4.0 * epsilon_c) {}
  inline void operator()(T &ret, const T &n4) const { ret = c1 + c2 * n4; }
};

struct AkFunctor {
  using T = Field2D::scalar_type;
  const T c1, c2;
  AkFunctor(const T &epsilon_k) : c1(1.0 + 3.0 * epsilon_k), c2(4.0 * epsilon_k) {}
  inline void operator()(T &ret, const T &n4) const { ret = c1 - c2 * n4; }
};

struct Aniso2Functor {
  using T = Field2D::scalar_type;
  const T c1;
  Aniso2Functor(const T &W0) : c1(std::pow(W0, 2.0)) {}
  inline void operator()(T &ret, const T &W, const T &dphi_dw) const {
    ret = (std::pow(W, 2.0) - c1) * dphi_dw;
  }
};

struct Aniso3Functor {
  using T = Field2D::scalar_type;
  const T c1;
  Aniso3Functor(const T &W0, const T &epsilon_c) : c1(16.0 * W0 * epsilon_c) {}
  inline void operator()(T &ret, const T &W, const T &dphi_dx, const T &dphi_dy,
                         const T &abs_n4_inv) const {
    ret = c1 * W *
          (std::pow(dphi_dx, 3.0) * std::pow(dphi_dy, 2.0) - (dphi_dx * std::pow(dphi_dy, 4.0))) *
          abs_n4_inv;
  }
};

struct TauInvFunctor {
  using T = Field2D::scalar_type;
  const T &tau0;
  TauInvFunctor(const T &t) : tau0(t) {}
  inline void operator()(T &ret, const T &ac, const T &ak) const {
    const T tmp = tau0 * ac * ak;
    ret = tmp == 0.0 ? 0.0 : 1.0 / tmp;
  }
};

struct ChemPotFunctor {
  using T = Field2D::scalar_type;
  const T &u;
  const T &lambda;
  ChemPotFunctor(const T &_u, const T &l) : u(_u), lambda(l) {}
  inline void operator()(T &ret, const T &phi) const {
    const T dw_pot = 1.0 - std::pow(phi, 2.0);
    ret = (phi - u * lambda * dw_pot) * dw_pot;
  }
};

struct PredictFunctor {
  using T = Field2D::scalar_type;
  const T &dt;
  PredictFunctor(const T &d) : dt(d) {}
  inline void operator()(T &next, const T &lhs_inv, const T &rhs, const T &prev) const {
    next = rhs * lhs_inv * dt + prev;
  }
};
} // namespace phase_field

#endif // __PHASE_FIELD__FUNCTOR__
