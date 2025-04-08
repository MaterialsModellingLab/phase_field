/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>

#include <phase_field/io.hh>
#include <phase_field/phase_field.hh>
#include <phase_field/util.hh>

struct Args {
  bool verbose = false;
  std::filesystem::path output = "output"; // *Optional
};

static void parse_args(const int argc, const char *const argv[], Args &args) {
  const auto &show_help = [argv](std::ostream &os) -> std::ostream & {
    os << argv[0] << " [Option]" << std::endl;
    os << std::endl;
    os << "Options:" << std::endl;
    os << "    --verbose  (Opt) Verbose mode" << std::endl;
    os << "    --output   (Opt) Output folder (default: output)" << std::endl;
    os << "    --help     (Opt) Print help" << std::endl;
    return os;
  };
  enum class Context {
    none = 0,
    output,
  } ctx = Context::none;

  for (int64_t i = 1; i < argc; ++i) {
    if (!strncmp(argv[i], "--", 2)) {
      ctx = Context::none;
      if (!strcmp(argv[i], "--verbose")) {
        args.verbose = true;
      } else if (!strcmp(argv[i], "--output")) {
        ctx = Context::output;
      } else if (!strcmp(argv[i], "--help")) {
        show_help(std::cout);
        exit(EXIT_SUCCESS);
      } else {
        throw std::invalid_argument(std::string("Invalid token '") + argv[i] + "' given");
      }
    } else {
      switch (ctx) {
      case Context::output: {
        args.output = argv[i];
        ctx = Context::none;
        break;
      }
      default:
        throw std::invalid_argument("Invalid token given");
      }
    }
  }

  if (ctx != Context::none) {
    std::clog << "required option for '" << argv[argc - 1] << "' not given" << std::endl;
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  Args args;
  try {
    parse_args(argc, argv, args);
  } catch (const std::invalid_argument &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  const auto fmt_filename = [](const std::size_t step) -> std::string {
    std::stringstream ss;
    ss << "pf_step";
    ss << std::setfill('0') << std::setw(6) << step;
    ss << ".dat";
    return ss.str();
  };
  const auto get_log_stream = [](const std::filesystem::path &filename) {
    std::ofstream fo{filename};
    if (!fo) {
      throw std::runtime_error("could not open '" + filename.string() + "'");
    }
    return fo;
  };

  if (!std::filesystem::is_directory(args.output) &&
      !std::filesystem::create_directories(args.output)) {
    std::cerr << "failed to create " << args.output << std::endl;
    return EXIT_FAILURE;
  }
  auto log_stream = get_log_stream(args.output / "out.log");

  phase_field::Param param = phase_field::get_pure_ni_param();

  param.lambda = 16.0;
  param.u = -0.2;
  param.setup();

  std::cout << param << std::endl;
  log_stream << param << std::endl;
  log_stream.close();

  const auto system = phase_field::PhaseField2D(param);
  phase_field::Field2D phi({80, 80});
  phase_field::set_nuclear_to_corner(phi, 10);
  if (args.verbose) {
    std::cout << "[Enter] to continue..." << std::endl;
    while (std::cin.get() != '\n')
      ;
  }

  auto phi_next = phase_field::Field2D::like(phi);
  for (std::size_t step = 0; step < 5000; ++step) {
    if (step % 100 == 0) {
      if (args.verbose) {
        std::system("clear");
        std::cout << "time: " << std::setw(6) << std::fixed << step * param.dt * 1.0e9 << " [ns] ";
        std::cout << "(step:" << std::setw(5) << step << ")" << std::endl;
        phase_field::io::fmt_visual_square(std::cout, phi) << std::endl;
      }
      phase_field::io::write(args.output / fmt_filename(step), phi, true);
    }
    system.predict(phi, phi_next);
    phi = phi_next;
  }

  std::cout << "Result is saved in " << args.output << std::endl;
  return EXIT_SUCCESS;
}
