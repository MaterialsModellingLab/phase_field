/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <benchmark/benchmark.h>
#include <phase_field/phase_field.hh>
#include <phase_field/util.hh>

static void BM_predict(benchmark::State &state) {
  phase_field::Param p = phase_field::get_pure_ni_param();
  p.lambda = 16.0;
  p.u = -0.2;
  p.setup();
  phase_field::PhaseField2D system(p);
  phase_field::Field2D phi({100, 100});
  auto phi_next = phase_field::Field2D::like(phi);
  for (auto _ : state) {
    state.PauseTiming();
    phase_field::set_nuclear_to_corner(phi, 10);
    state.ResumeTiming();
    system.predict(phi, phi_next);
  }
}
BENCHMARK(BM_predict)->Iterations(1000);

BENCHMARK_MAIN();