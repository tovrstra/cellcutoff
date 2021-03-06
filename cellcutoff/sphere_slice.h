// CellCutoff is a library for periodic boundary conditions and real-space cutoff calculations.
// Copyright (C) 2017 The CellCutoff Development Team
//
// This file is part of CellCutoff.
//
// CellCutoff is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or (at your option) any later version.
//
// CellCutoff is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>
//
// --

/** @file */


#ifndef CELLCUTOFF_SPHERE_SLICE_H_
#define CELLCUTOFF_SPHERE_SLICE_H_

#include <stdexcept>
#include <string>


namespace cellcutoff {


/** @brief.
        An exception for when solve_range(_*) can not find a solution.
 */
class no_solution_found : public std::domain_error {
 public:
  explicit no_solution_found(const std::string& what_arg)
      : std::domain_error(what_arg) {}
};


class SphereSlice {
 public:
  SphereSlice(const double* center, const double* normals, double radius);

  /* Move-constructor and assignment make no sense as the SphereSlice is
     constant after construction! Just pass a reference or a pointer instead. If needed,
     a copy can be made to guarantee that the data remains available. */
  SphereSlice(SphereSlice&&) = delete;
  SphereSlice& operator=(const SphereSlice&) = delete;

  // Main API
  void solve_range(const int ncut, double* begin, double* end) const;
  void set_cut_begin_end(const int icut, double new_begin, double new_end);

  // Auxiliary API, could also be useful and there is no need to really
  // make this private. Having it public also facilitates testing.
  void solve_range_0(double* begin, double* end) const;
  void solve_range_1(double* begin, double* end) const;
  void solve_range_2(double* begin, double* end) const;

  void solve_full(const int id_axis, double* begin, double* end,
      const int id_cut0 = -1, const int id_cut1 = -1) const;
  void solve_full_low(const int id_axis, double* begin, double* end,
      double* point_begin = nullptr, double* point_end = nullptr) const;

  void solve_plane(const int id_axis, const int id_cut0, const double frac_cut,
      double* begin, double* end, const int id_cut1 = -1) const;
  void solve_plane_low(const int id_axis, const int id_cut, const double frac_cut,
      double* begin, double* end,
      double* point_begin = nullptr, double* point_end = nullptr) const;

  void solve_line(const int id_axis, const int id_cut0, const int id_cut1,
      const double frac_cut0, const double frac_cut1, double* begin, double* end) const;
  void solve_line_low(const int id_axis, const int id_cut0, const int id_cut1,
      const double frac_cut0, const double frac_cut1, double* begin, double* end,
      double* point_begin = nullptr, double* point_end = nullptr) const;
  double compute_plane_intersection(const int id_cut0, int const id_cut1,
      const double cut0, const double cut1, double* other_center) const;

  bool inside_cuts(const int id_cut, const double* point) const;

 private:
  // Constant independent data members
  const double* center_;
  const double* normals_;
  const double radius_;

  // Configurable data members
  double cut_begin[2];
  double cut_end[2];

  // Derived from constant data members upon construction
  double radius_sq_;
  double norms_sq_[3];
  double norms_[3];
  double frac_radii_[3];
  double frac_center_[3];
  double sphere_frac_begin_[3];
  double sphere_frac_end_[3];
  double radius_normals_[9];
  double sphere_point_begin_[9];
  double sphere_point_end_[9];
  double dots_[9];
  double denoms_[9];
  double cut_ortho_[27];
};

void compute_begin_end(const double* other_center, const double* ortho,
    const double* axis, double* begin, double* end,
    double* point_begin, double* point_end);

void update_begin_end(const double work_begin, const double work_end,
    double* begin, double* end);


}  // namespace cellcutoff


#endif  // CELLCUTOFF_SPHERE_SLICE_H_

// vim: textwidth=90 et ts=2 sw=2
