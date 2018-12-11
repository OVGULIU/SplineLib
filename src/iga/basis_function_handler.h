/* Copyright 2018 Chair for Computational Analysis of Technical Systems, RWTH Aachen University

This file is part of SplineLib.

SplineLib is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation version 3 of the License.

SplineLib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with SplineLib.  If not, see
<http://www.gnu.org/licenses/>.
*/

#ifndef SRC_IGA_BASIS_FUNCTION_HANDLER_H_
#define SRC_IGA_BASIS_FUNCTION_HANDLER_H_

#include <math.h>
#include <armadillo>
#include <array>
#include <vector>

#include "connectivity_handler.h"
#include "element.h"
#include "element_generator.h"
#include "element_integration_point.h"
#include "integration_rule.h"
#include "mapping_handler.h"
#include "multi_index_handler.h"
#include "nurbs.h"

namespace iga {
template<int DIM>
class BasisFunctionHandler {
 public:
  explicit BasisFunctionHandler(std::shared_ptr<spl::NURBS<DIM>> spl) : spline_(std::move(spl)) {
    mapping_handler_ = std::make_shared<iga::MappingHandler<DIM>>(spline_);
    element_generator_ = std::make_shared<iga::elm::ElementGenerator<DIM>>(spline_);
  }

  std::vector<iga::elm::ElementIntegrationPoint> EvaluateAllElementNonZeroNURBSBasisFunctions(
      int element_number, const iga::itg::IntegrationRule &rule) const {
    std::vector<iga::elm::ElementIntegrationPoint> element_integration_points;
    for (auto &itg_pnt_eta : rule.GetIntegrationPoints()) {
      for (auto &itg_pnt_xi : rule.GetIntegrationPoints()) {
        std::array<ParamCoord, 2> param_coords = mapping_handler_->Reference2ParameterSpace(
            element_number, {itg_pnt_xi.GetCoordinate(), itg_pnt_eta.GetCoordinate()});
        element_integration_points.emplace_back(iga::elm::ElementIntegrationPoint(
            EvaluateAllNonZeroNURBSBasisFunctions(param_coords), itg_pnt_xi.GetWeight() * itg_pnt_eta.GetWeight(),
            mapping_handler_->GetJacobianDeterminant(param_coords)));
      }
    }
    return element_integration_points;
  }

  std::vector<iga::elm::ElementIntegrationPoint> EvaluateAllElementNonZeroNURBSBasisFunctionDerivatives(
      int element_number, const iga::itg::IntegrationRule &rule) const {
    std::vector<iga::elm::ElementIntegrationPoint> element_integration_points;
    for (auto &itg_pnt_eta : rule.GetIntegrationPoints()) {
      for (auto &itg_pnt_xi : rule.GetIntegrationPoints()) {
        std::array<ParamCoord, 2> param_coords = mapping_handler_->Reference2ParameterSpace(
            element_number, {itg_pnt_xi.GetCoordinate(), itg_pnt_eta.GetCoordinate()});
        element_integration_points.emplace_back(iga::elm::ElementIntegrationPoint(
            EvaluateAllNonZeroNURBSBasisFunctionDerivatives(param_coords), itg_pnt_xi.GetWeight()
                * itg_pnt_eta.GetWeight(), mapping_handler_->GetJacobianDeterminant(param_coords)));
      }
    }
    return element_integration_points;
  }

  std::vector<iga::elm::ElementIntegrationPoint> EvaluateAllElementNonZeroNURBSBafDerivativesPhysical(
      int element_number, const iga::itg::IntegrationRule &rule) const {
    std::vector<iga::elm::ElementIntegrationPoint> element_integration_points;
    for (auto &itg_pnt_eta : rule.GetIntegrationPoints()) {
      for (auto &itg_pnt_xi : rule.GetIntegrationPoints()) {
        std::array<ParamCoord, 2> param_coords = mapping_handler_->Reference2ParameterSpace(
            element_number, {itg_pnt_xi.GetCoordinate(), itg_pnt_eta.GetCoordinate()});
        element_integration_points.emplace_back(iga::elm::ElementIntegrationPoint(
            EvaluateAllNonZeroNURBSBafDerivativesPhysical(param_coords), itg_pnt_xi.GetWeight() * itg_pnt_eta.GetWeight(),
            mapping_handler_->GetJacobianDeterminant(param_coords)));
      }
    }
    return element_integration_points;
  }

 private:
  std::vector<double> EvaluateAllNonZeroNURBSBasisFunctions(std::array<ParamCoord, DIM> param_coord) const {
    std::array<std::vector<double>, DIM> basis_functions{};
    std::array<int, DIM> num_baf{};
    for (int i = 0; i < DIM; ++i) {
      basis_functions[i] = spline_->EvaluateAllNonZeroBasisFunctions(i, param_coord[i]);
      num_baf[i] = basis_functions[i].size();
    }
    std::vector<double> nurbs_basis_functions;
    double sum = 0;
    util::MultiIndexHandler<DIM> mih(num_baf);
    while (true) {
      double temp = 1;
      for (int i = 0; i < DIM; ++i) {
        temp *= basis_functions[i][mih[i]];
      }
      nurbs_basis_functions.emplace_back(temp * GetWeight(param_coord, mih.Get1DIndex()));
      sum += temp * GetWeight(param_coord, mih.Get1DIndex());
      if (mih.Get1DIndex() == mih.Get1DLength() - 1) break;
      ++mih;
    }
    for (auto &nbaf : nurbs_basis_functions) {
      nbaf = nbaf / sum;
    }
    return nurbs_basis_functions;
  }

  std::array<std::vector<double>, 2> EvaluateAllNonZeroNURBSBasisFunctionDerivatives(
      std::array<ParamCoord, 2> param_coord) const {
    std::array<std::vector<double>, 2> basis_functions = std::array<std::vector<double>, 2>({
      spline_->EvaluateAllNonZeroBasisFunctions(0, param_coord[0]),
      spline_->EvaluateAllNonZeroBasisFunctions(1, param_coord[1])});
    std::array<std::vector<double>, 2> basis_function_derivatives = std::array<std::vector<double>, 2>({
      spline_->EvaluateAllNonZeroBasisFunctionDerivatives(0, param_coord[0], 1),
      spline_->EvaluateAllNonZeroBasisFunctionDerivatives(1, param_coord[1], 1)});
    std::vector<double> nurbs_basis_functions;
    std::array<std::vector<double>, 2> nurbs_basis_function_derivatives;
    double sum_baf = 0;
    double sum_der_xi = 0;
    double sum_der_eta = 0;
    int l = 0;
    for (uint64_t i = 0; i < basis_function_derivatives[1].size(); ++i) {
      for (uint64_t j = 0; j < basis_function_derivatives[0].size(); ++j) {
        double temp = basis_functions[0][j] * basis_functions[1][i] * GetWeight(param_coord, l);
        double temp_der_xi = basis_function_derivatives[0][j] * basis_functions[1][i] * GetWeight(param_coord, l);
        double temp_der_eta = basis_functions[0][j] * basis_function_derivatives[1][i] * GetWeight(param_coord, l);
        sum_baf += temp;
        sum_der_xi += temp_der_xi;
        sum_der_eta += temp_der_eta;
        nurbs_basis_functions.emplace_back(temp);
        nurbs_basis_function_derivatives[0].emplace_back(temp_der_xi);
        nurbs_basis_function_derivatives[1].emplace_back(temp_der_eta);
        l += 1;
      }
    }
    for (uint64_t i = 0; i < basis_functions.size(); ++i) {
      nurbs_basis_functions[i] = nurbs_basis_functions[i] / sum_baf;
      nurbs_basis_function_derivatives[0][i] = (nurbs_basis_function_derivatives[0][i] * sum_baf -
          nurbs_basis_functions[i] * sum_der_xi) / pow(sum_baf, 2);
      nurbs_basis_function_derivatives[1][i] = (nurbs_basis_function_derivatives[1][i] * sum_baf -
          nurbs_basis_functions[i] * sum_der_eta) / pow(sum_baf, 2);
    }
    return nurbs_basis_function_derivatives;
  }

  std::array<std::vector<double>, DIM> EvaluateAllNonZeroNURBSBafDerivativesPhysical(
      std::array<ParamCoord, DIM> param_coord) const {
    std::array<std::vector<double>, DIM> dr_dx;
    std::array<std::vector<double>, DIM> dr_dxi = EvaluateAllNonZeroNURBSBasisFunctionDerivatives(param_coord);
    arma::dmat dxi_dx = mapping_handler_->GetDxiDx(param_coord);
    for (uint64_t j = 0; j < dr_dxi[0].size(); ++j) {
      dr_dx[0].emplace_back(dr_dxi[0][j] * dxi_dx(0, 0) + dr_dxi[1][j] * dxi_dx(1, 0));
      dr_dx[1].emplace_back(dr_dxi[0][j] * dxi_dx(0, 1) + dr_dxi[1][j] * dxi_dx(1, 1));
    }
    return dr_dx;
  }


  double GetWeight(std::array<ParamCoord, DIM> param_coord, int local_index) const {
    iga::ConnectivityHandler<DIM> connectivity_handler(spline_);
    return spline_->GetWeights()[connectivity_handler.GetGlobalIndex(element_generator_->GetElementNumberAtParamCoord(
        param_coord), local_index) - 1];
  }

  std::shared_ptr<spl::NURBS<DIM>> spline_;
  std::shared_ptr<iga::MappingHandler<DIM>> mapping_handler_;
  std::shared_ptr<iga::elm::ElementGenerator<DIM>> element_generator_;
};
}  // namespace iga

#endif  // SRC_IGA_BASIS_FUNCTION_HANDLER_H_
