/* Copyright 2018 Chair for Computational Analysis of Technical Systems, RWTH Aachen University

This file is part of SplineLib.

SplineLib is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation version 3 of the License.

SplineLib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with SplineLib.  If not, see
<http://www.gnu.org/licenses/>.
*/

#ifndef SRC_IGA_MAPPING_HANDLER_H_
#define SRC_IGA_MAPPING_HANDLER_H_

#include <armadillo>

#include "element_generator.h"
#include "spline.h"

namespace iga {
template<int DIM>
class MappingHandler {
 public:
  explicit MappingHandler(std::shared_ptr<spl::Spline<DIM>> spl) : spline_(std::move(spl)) {}

  arma::dmat GetDxiDx(std::array<ParamCoord, DIM> param_coord) const {
    return GetDxDxi(param_coord).i();
  }

  double GetJacobianDeterminant(std::array<ParamCoord, DIM> param_coord) const {
    return arma::det(GetDxDxitilde(param_coord));
  }

  std::array<ParamCoord, DIM> Reference2ParameterSpace(int element_number, std::array<double, DIM> itg_pnt) const {
    iga::elm::ElementGenerator<DIM> elm_gen(spline_);
    iga::elm::Element element_xi = elm_gen.GetElementList(0)[elm_gen.GetElementIndices(element_number)[0]];
    iga::elm::Element element_eta = elm_gen.GetElementList(1)[elm_gen.GetElementIndices(element_number)[1]];
    ParamCoord upper_xi = element_xi.GetNode(1);
    ParamCoord lower_xi = element_xi.GetNode(0);
    ParamCoord upper_eta = element_eta.GetNode(1);
    ParamCoord lower_eta = element_eta.GetNode(0);
    return {ParamCoord{((upper_xi - lower_xi).get() * itg_pnt[0] + (upper_xi + lower_xi).get()) / 2.0},
            ParamCoord{((upper_eta - lower_eta).get() * itg_pnt[1] + (upper_eta + lower_eta).get()) / 2.0}};
  }

 private:
  arma::dmat GetDxDxitilde(std::array<ParamCoord, DIM> param_coord) const {
    arma::dmat dx_dxitilde(static_cast<uint64_t>(DIM), static_cast<uint64_t>(DIM), arma::fill::zeros);
    arma::dmat dx_dxi = GetDxDxi(param_coord);
    arma::dmat dxi_dxitilde = GetDxiDxitilde(param_coord);
    for (uint64_t i = 0; i < 2; ++i) {
      for (uint64_t j = 0; j < 2; ++j) {
        dx_dxitilde(i, j) = dx_dxi(i, j) * dxi_dxitilde(j, j);
      }
    }
    return dx_dxitilde;
  }

  arma::dmat GetDxDxi(std::array<ParamCoord, DIM> param_coord) const {
    arma::dmat dx_dxi(static_cast<uint64_t>(DIM), static_cast<uint64_t>(DIM), arma::fill::zeros);
    for (int i = 0; i < DIM; ++i) {
      for (int j = 0; j < DIM; ++j) {
        std::array<int, DIM> derivative{};
        derivative[j] = 1;
        dx_dxi(static_cast<uint64_t>(i), static_cast<uint64_t>(j)) =
            spline_->EvaluateDerivative(param_coord, {i}, derivative)[0];
      }
    }
    return dx_dxi;
  }

  arma::dmat GetDxiDxitilde(std::array<ParamCoord, DIM> param_coord) const {
    arma::dmat dxi_dxitilde(static_cast<uint64_t>(DIM), static_cast<uint64_t>(DIM), arma::fill::zeros);
    std::array<size_t, DIM> knot_span{};
    for (int i = 0; i < DIM; ++i) {
      knot_span[i] = static_cast<size_t>(spline_->GetKnotVector(i)->GetKnotSpan(param_coord[i]).get());
      dxi_dxitilde(static_cast<uint64_t>(i), static_cast<uint64_t>(i)) =
          (spline_->GetKnotVector(i)->GetKnot(knot_span[i] + 1).get()
          - spline_->GetKnotVector(i)->GetKnot(knot_span[i]).get()) / 2;
    }
    return dxi_dxitilde;
  }

  std::shared_ptr<spl::Spline<DIM>> spline_;
};
}  // namespace iga

#endif  // SRC_IGA_MAPPING_HANDLER_H_
