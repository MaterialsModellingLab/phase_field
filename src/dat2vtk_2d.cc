/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>

#include <phase_field/io.hh>

#include <vtkImageData.h>
#include <vtkNew.h>
#include <vtkXMLImageDataWriter.h>

struct Args {
  std::filesystem::path input;
  std::filesystem::path output = "vtk"; // *Optional
  std::filesystem::path name;           // *Optional
};

static void parse_args(const int argc, const char *const argv[], Args &args) {
  const auto &show_help = [argv](std::ostream &os) -> std::ostream & {
    os << argv[0] << " [Option]" << std::endl;
    os << std::endl;
    os << "Options:" << std::endl;
    os << "    --input    Input .dat file" << std::endl;
    os << "    --output   (Opt) Output path (default: vtk)" << std::endl;
    os << "    --name     (Opt) Output filename (default: <input_basename>.vti)" << std::endl;
    os << "    --help     (Opt) Print help" << std::endl;
    return os;
  };
  enum class context {
    none = 0,
    input,
    output,
    name,
  } ctx = context::none;

  for (int64_t i = 1; i < argc; ++i) {
    if (!strncmp(argv[i], "--", 2)) {
      ctx = context::none;
      if (!strcmp(argv[i], "--input")) {
        ctx = context::input;
      } else if (!strcmp(argv[i], "--output")) {
        ctx = context::output;
      } else if (!strcmp(argv[i], "--name")) {
        ctx = context::name;
      } else if (!strcmp(argv[i], "--help")) {
        show_help(std::cout);
        exit(EXIT_SUCCESS);
      } else {
        throw std::invalid_argument(std::string("Invalid token '") + argv[i] + "' given");
      }
    } else {
      switch (ctx) {
      case context::input: {
        args.input = argv[i];
        ctx = context::none;
        break;
      }
      case context::output: {
        args.output = argv[i];
        ctx = context::none;
        break;
      }
      case context::name: {
        args.name = argv[i];
        ctx = context::none;
        break;
      }
      default:
        throw std::invalid_argument("Invalid token given");
      }
    }
  }

  if (args.input.empty()) {
    throw std::invalid_argument("Required option '--input' not given");
  }

  if (args.name.empty()) {
    args.name = std::filesystem::path(args.input).filename().replace_extension(".vti");
  }
}

int main(int argc, char *argv[]) {
  Args args;
  try {
    parse_args(argc, argv, args);
  } catch (const std::invalid_argument &e) {
    std::clog << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  if (!std::filesystem::is_directory(args.output) &&
      !std::filesystem::create_directories(args.output)) {
    std::cerr << "failed to create " << args.output << std::endl;
    return EXIT_FAILURE;
  }

  phase_field::Field2D field;
  try {
    phase_field::io::read(args.input, field);
  } catch (const std::runtime_error &e) {
    std::clog << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkImageData> image_data;
  image_data->SetOrigin(0, 0, 0);
  image_data->SetSpacing(1, 1, 1);

  const std::size_t size_y = field.shape()[0];
  const std::size_t size_x = field.shape()[1];

  // Make double size for mirror flip
  image_data->SetDimensions(size_x * 2, size_y * 2, 1);
  image_data->AllocateScalars(VTK_DOUBLE, 1);

  const auto get_at = [&image_data](const std::size_t x,
                                    const std::size_t y) -> phase_field::Field2D::scalar_type * {
    return static_cast<phase_field::Field2D::scalar_type *>(image_data->GetScalarPointer(x, y, 0));
  };

  for (std::size_t y = 0; y < size_y; ++y) {
    for (std::size_t x = 0; x < size_x; ++x) {
      // Flip for 4 fold sym
      auto px_quad1 = get_at(size_x + x, size_y + y);
      auto px_quad2 = get_at(size_x - 1 - x, size_x + y);
      auto px_quad3 = get_at(size_x - 1 - x, size_y - 1 - y);
      auto px_quad4 = get_at(size_x + x, size_y - 1 - y);
      *px_quad1 = field[y][x];
      *px_quad2 = field[y][x];
      *px_quad3 = field[y][x];
      *px_quad4 = field[y][x];
    }
  }

  vtkNew<vtkXMLImageDataWriter> writer;
  writer->SetDataModeToBinary();
  writer->SetFileName((args.output / args.name).c_str());
  writer->SetInputData(image_data);
  writer->Write();

  std::cout << args.output / args.name << std::endl;

  return EXIT_SUCCESS;
}