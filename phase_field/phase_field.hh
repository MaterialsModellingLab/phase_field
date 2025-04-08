/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PHASE_FIELD__PHASE_FIELD__
#define __PHASE_FIELD__PHASE_FIELD__

#include "impl/derivative.hh"
#include "impl/functor.hh"
#include "impl/state.hh"
#include "impl/type.hh"
#include "param.hh"
#include <algorithm>

namespace phase_field {

class PhaseField2D {
public:
  const Param param;

private:
  InvAbsN4Functor inv_abs_n4_func;
  N4Functor n4_func;
  AcFunctor ac_func;
  AkFunctor ak_func;
  libtensor::functor::BindLhsWrapper<libtensor::functor::ProdFunctor<Field2D::scalar_type>> w_func;
  Aniso2Functor aniso2_func;
  Aniso3Functor aniso3_func;
  ChemPotFunctor chem_func;
  FieldClampFunctor clamp_func;
  libtensor::functor::SumFunctor<Field2D::scalar_type> sum_func;
  PredictFunctor predict_func;

public:
  PhaseField2D(const Param &p)
      : param(p), ac_func(p.epsilon_c), ak_func(p.epsilon_k), w_func(p.W0), aniso2_func(p.W0),
        aniso3_func(p.W0, p.epsilon_c), chem_func(param.u, param.lambda), predict_func(param.dt) {}

  inline void predict(const Field2D &phi, Field2D &ret) const {
    if (phi.shape() != ret.shape()) {
      throw std::runtime_error("invalid shape of tensor given");
    }
    static const auto dx_filter = get_dx_filter(param.dx);
    static const auto dy_filter = get_dy_filter(param.dx);
    static const auto lap_filter = get_laplacian_filter(param.dx);

    static auto dphi_dx = Field2D::like(phi);
    static auto dphi_dy = Field2D::like(phi);
    conv2d(phi, dx_filter, dphi_dx);
    conv2d(phi, dy_filter, dphi_dy);

    static auto abs_n4_inv = Field2D::like(phi);
    abs_n4_inv.map(inv_abs_n4_func, dphi_dx, dphi_dy);

    static auto n4 = Field2D::like(phi);
    n4.map(n4_func, dphi_dx, dphi_dy, abs_n4_inv);

    static auto ac = Field2D::like(phi);
    static auto ak = Field2D::like(phi);
    ac.map(ac_func, n4);
    ak.map(ak_func, n4);

    static auto W = Field2D::like(phi);
    W.map(w_func, ac);

    static auto tau_inv = Field2D::like(phi);
    tau_inv.map(TauInvFunctor(param.tau0), ac, ak);

    static auto term1 = Field2D::like(phi);
    static auto term2_dx = Field2D::like(phi);
    static auto term2_dy = Field2D::like(phi);
    static auto term3_dx = Field2D::like(phi);
    static auto term3_dy = Field2D::like(phi);
    static auto term4 = Field2D::like(phi);

    static auto cache = Field2D::like(phi);

    conv2d(phi, lap_filter, term1);
    term1.map([&](Field2D::scalar_type &ret) { ret *= std::pow(param.W0, 2.0); });
    conv2d(cache.map(aniso2_func, W, dphi_dx), dx_filter, term2_dx);
    conv2d(cache.map(aniso2_func, W, dphi_dy), dy_filter, term2_dy);
    conv2d(cache.map(aniso3_func, W, dphi_dx, dphi_dy, abs_n4_inv), dx_filter, term3_dx);
    conv2d(cache.map(aniso3_func, W, dphi_dy, dphi_dx, abs_n4_inv), dy_filter, term3_dy);
    term4.map(chem_func, phi);
    cache.map(sum_func, term1, term2_dx, term2_dy, term3_dx, term3_dy, term4);

    ret.map(predict_func, tau_inv, cache, phi).map(clamp_func);
    return;
  }
};
} // namespace phase_field

#endif