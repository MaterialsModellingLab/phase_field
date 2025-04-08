/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PHASE_FIELD__PARAM__
#define __PHASE_FIELD__PARAM__

#include "impl/type.hh"
#include <cassert>
#include <cmath>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <ostream>

namespace phase_field {
inline constexpr Field2D::scalar_type a1 = 5.0 * M_SQRT2 / 8.0;
inline constexpr Field2D::scalar_type kb = 1.380649e-23;

struct Param {
  /* Physical Parameter */
  Field2D::scalar_type Tm;
  Field2D::scalar_type L;
  Field2D::scalar_type cp;
  Field2D::scalar_type d0;
  Field2D::scalar_type gamma0; // NOTE: Not used in simulation
  Field2D::scalar_type beta0;  // From MD
  Field2D::scalar_type epsilon_c;
  Field2D::scalar_type epsilon_k; // From MD

  /* User Input */
  Field2D::scalar_type u;
  Field2D::scalar_type lambda;

  /* Computed from the parameter above */
  Field2D::scalar_type T;
  Field2D::scalar_type W0;
  Field2D::scalar_type tau0;

  /* Defined for stability */
  Field2D::scalar_type dx;
  Field2D::scalar_type dt;

  NLOHMANN_DEFINE_TYPE_INTRUSIVE(Param, Tm, L, cp, d0, gamma0, beta0, epsilon_c, epsilon_k, u,
                                 lambda);

  inline void setup() {
    this->T = calc_T(*this);
    this->W0 = calc_W0(*this);
    this->tau0 = calc_tau0(*this);
    this->dx = calc_dx(*this);
    this->dt = calc_dt(*this);
  }

  inline friend std::ostream &operator<<(std::ostream &os, const Param &p) {
    const auto fmt = [&os](const std::string &&title, const Field2D::scalar_type &val,
                           const std::string &&unit = "") {
      os << "  ";                                      // tab
      os << std::left << std::setw(10) << title + ":"; // title
      const auto exponent = std::floor(std::log10(std::abs(val)));
      os << std::right << std::setw(12);
      if (std::abs(exponent) <= 3) {
        os << std::fixed << std::setprecision(3) << val;
      } else {
        os << std::scientific << std::setprecision(3) << val;
      }
      if (!unit.empty()) {
        os << " [" + unit + "]"; // unit
      }
    };
    fmt("Tm", p.Tm, "K"), os << std::endl;
    fmt("L", p.L, "J/m^3"), os << std::endl;
    fmt("cp", p.cp, "J/(m^3K)"), os << std::endl;
    fmt("d0", p.d0, "m"), os << std::endl;
    fmt("gamma0", p.gamma0, "J/m^2"), os << std::endl;
    fmt("beta0", p.beta0, "sec/m"), os << std::endl;
    fmt("epsilon_c", p.epsilon_c), os << std::endl;
    fmt("epsilon_k", p.epsilon_k), os << std::endl;
    fmt("<u>", p.u), os << std::endl;
    fmt("<lambda>", p.lambda), os << std::endl;
    fmt("(Fexpt)", Param::calc_Fexpt(p)), os << std::endl;
    fmt("(T)", p.T, "K"), os << std::endl;
    fmt("(W0)", p.W0), os << std::endl;
    fmt("(tau0)", p.tau0), os << std::endl;
    fmt("(dx)", p.dx, "m"), os << std::endl;
    fmt("(dt)", p.dt, "sec");
    return os;
  }

  static inline Field2D::scalar_type calc_u(const Param &p) {
    assert(p.T && "Temperature not set");
    return (p.Tm - p.T) / (p.L / p.cp);
  }

  static inline Field2D::scalar_type calc_T(const Param &p) {
    assert(p.u && "Temperature not set");
    return p.Tm + p.u * p.L / p.cp;
  }

  static inline Field2D::scalar_type calc_W0(const Param &p) {
    assert(p.lambda && "Lambda not set");
    return p.lambda * p.d0 / a1;
  }
  static inline Field2D::scalar_type calc_tau0(const Param &p) {
    assert(p.lambda && "Lambda not set");
    return std::pow(p.lambda, 2.0) * p.d0 * p.beta0 / std::pow(a1, 2.0);
  }
  static inline Field2D::scalar_type calc_dx(const Param &p) {
    assert(p.W0 && "W0 not set");
    return 0.8 * p.W0;
  }
  static inline Field2D::scalar_type calc_dt(const Param &p) {
    assert(p.tau0 && "tau0 not set");
    return 0.004 * p.tau0;
  }
  static inline Field2D::scalar_type calc_Fexpt(const Param &p) {
    return kb * std::pow(p.Tm, 2.0) * p.cp / std::pow(p.L, 2.0) / std::pow(p.d0, 3.0);
  }
};

static inline const auto get_pure_ni_param = []() -> Param {
  Param ret;
  ret.Tm = 1726;
  ret.L = 2.311e9;
  ret.cp = 5.313e6;
  ret.d0 = 5.56e-10;
  ret.gamma0 = 0.326;
  ret.beta0 = 5.084e-3;
  ret.epsilon_c = 0.018;
  ret.epsilon_k = 0.13;
  return ret;
};
} // namespace phase_field

#endif
