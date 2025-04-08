/*
 * Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PHASE_FIELD__IO__
#define __PHASE_FIELD__IO__

#include "impl/color.hh"
#include "impl/state.hh"
#include "impl/type.hh"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include <sstream>

namespace phase_field::io {
/**
 * @brief Print phase field in raw format
 *  Red:    solid
 *  Green:  intermediate
 *  Cyan:   liquid
 *
 * @param os output stream
 * @param field phase field in 2D tensor
 * @return std::ostream& output stream
 */
inline std::ostream &fmt_raw(std::ostream &os, const Field2D &field, const char delimiter = ' ') {
  const auto &f_shape = field.shape();
  for (int64_t y = 0; y < static_cast<int64_t>(f_shape[0]); ++y) {
    const auto &f = field[y];
    for (int64_t x = 0; x < static_cast<int64_t>(f_shape[1]); ++x) {
      const auto &val = f[x];
      os << val;
      if (x < static_cast<int64_t>(f_shape[1]) - 1) {
        os << delimiter;
      }
    }
    if (y < static_cast<int64_t>(f_shape[0]) - 1) {
      os << std::endl;
    }
  }
  return os;
}

inline auto color_prefix(const Field2D::scalar_type &val) {
  static const Field2D::scalar_type threshold = 0.8;
  if (val >= FieldState::solid * threshold) {
    return color::RED;
  } else if (val <= FieldState::liquid * threshold) {
    return color::CYN;
  } else if (val >= 0.0) {
    return color::YEL;
  } else {
    return color::GRN;
  }
}

/**
 * @brief Print phase field in visual format with digit
 *  Red:    solid
 *  Yellow: intermediate >= 0
 *  Green:  intermediate < 0
 *  Cyan:   liquid
 *
 * @param os output stream
 * @param field phase field in 2D tensor
 * @return std::ostream& output stream
 */
inline std::ostream &fmt_visual(std::ostream &os, const Field2D &field) {
  const auto &f_shape = field.shape();
  /* NOTE: Internal data will be flipped in Y direction for cartesian coordinate */
  for (int64_t y = f_shape[0] - 1; y >= 0; --y) {
    for (int64_t x = 0; x < static_cast<int64_t>(f_shape[1]); ++x) {
      const auto &val = field[y][x];
      os << color_prefix(val);
      os << std::scientific << std::setprecision(1) << std::setw(9);
      os << val;
      os << color::RST;
    }
    if (y > 0) {
      os << std::endl;
    }
  }
  return os;
}

/**
 * @brief Print phase field in visual format with ■
 *  Red:    solid
 *  Yellow: intermediate >= 0
 *  Green:  intermediate < 0
 *  Cyan:   liquid
 *
 * @param os output stream
 * @param field phase field in 2D tensor
 * @return std::ostream& output stream
 */
inline std::ostream &fmt_visual_square(std::ostream &os, const Field2D &field) {
  const auto &f_shape = field.shape();
  static const int64_t dsp_y = 32;
  static const int64_t dsp_x = 32;
  const int64_t sp_y = f_shape[0] / dsp_y;
  const int64_t sp_x = f_shape[0] / dsp_x;
  /* NOTE: Internal data will be flipped in Y direction for cartesian coordinate */
  for (int64_t y = dsp_y - 1; y >= 0; --y) {
    const auto &f = field[y * sp_y];
    for (int64_t x = 0; x < dsp_x; ++x) {
      const auto val = f[x * sp_x];
      os << color_prefix(val);
      os << std::setw(4);
      os << "■";
      os << color::RST;
    }
    if (y > 0) {
      os << std::endl;
    }
  }
  return os;
}

inline void write(const std::filesystem::path &filename, const Field2D &field,
                  const bool force = false) {
  if (std::filesystem::exists(filename) && !force) {
    throw std::runtime_error("file '" + filename.string() + "' already exist");
  }

  std::ofstream fo{filename};
  if (!fo) {
    throw std::runtime_error("could not open '" + filename.string() + "'");
  }
  fmt_raw(fo, field) << std::endl;
}

inline void read(const std::filesystem::path &filename, Field2D &field) {
  const auto get_data = [](std::istream &is) {
    std::vector<std::vector<Field2D::scalar_type>> ret;
    std::string l1;
    while (std::getline(is, l1)) {
      std::string l2;
      std::stringstream ss{l1};
      ret.emplace_back(std::vector<Field2D::scalar_type>{});
      while (std::getline(ss, l2, ' ')) {
        ret.back().emplace_back(std::stod(l2));
      }
    }
    return ret;
  };

  if (!std::filesystem::exists(filename)) {
    throw std::runtime_error("file '" + filename.string() + "' not exist");
  }

  if (!std::filesystem::is_regular_file(filename)) {
    throw std::runtime_error("file '" + filename.string() + "' is not regular file");
  }

  std::ifstream fi{filename};
  if (!fi) {
    throw std::runtime_error("could not open '" + filename.string() + "'");
  }

  const auto data = get_data(fi);
  const std::size_t y_size = data.size();
  const std::size_t x_size = data.front().size();
  field.resize({y_size, x_size});
#pragma omp parallel for
  for (std::size_t y = 0; y < y_size; ++y) {
#pragma omp parallel for
    for (std::size_t x = 0; x < x_size; ++x) {
      field[y][x] = data[y][x];
    }
  }
}
} // namespace phase_field::io

#endif