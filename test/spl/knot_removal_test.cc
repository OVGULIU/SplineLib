/* Copyright 2018 Chair for Computational Analysis of Technical Systems, RWTH Aachen University

This file is part of SplineLib.

SplineLib is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation version 3 of the License.

SplineLib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with SplineLib.  If not, see
<http://www.gnu.org/licenses/>.
*/

#include "gmock/gmock.h"

#include "b_spline.h"
#include "nurbs.h"
#include "random_b_spline_generator.h"
#include "random_nurbs_generator.h"
#include "irit_writer.h"

using testing::Test;
using testing::DoubleEq;
using testing::DoubleNear;

class BSplineFig5_26 : public Test {  // NOLINT
 public:
  BSplineFig5_26() {
    std::array<Degree, 1> degree = {Degree{3}};
    KnotVectors<1> knot_vector_before = {std::make_shared<baf::KnotVector>(
        baf::KnotVector({ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{1}, ParamCoord{1},
                         ParamCoord{1}, ParamCoord{2}, ParamCoord{2}, ParamCoord{2}, ParamCoord{2}}))};
    std::vector<baf::ControlPoint> control_points = {
        baf::ControlPoint(std::vector<double>({0.0, 0.0})),
        baf::ControlPoint(std::vector<double>({0.0, 1.5})),
        baf::ControlPoint(std::vector<double>({1.0, 2.0})),
        baf::ControlPoint(std::vector<double>({2.0, 2.0})),
        baf::ControlPoint(std::vector<double>({3.0, 2.0})),
        baf::ControlPoint(std::vector<double>({4.0, 1.5})),
        baf::ControlPoint(std::vector<double>({4.0, 0.0}))
    };
    bspline_1d_before_ = std::make_shared<spl::BSpline<1>>(knot_vector_before, degree, control_points);
    spl::BSpline<1> b_spline_after(*bspline_1d_before_);
    bspline_1d_after_ = std::make_shared<spl::BSpline<1>>(b_spline_after);
  }

 protected:
  std::shared_ptr<spl::BSpline<1>> bspline_1d_before_;
  std::shared_ptr<spl::BSpline<1>> bspline_1d_after_;
};

TEST_F(BSplineFig5_26, RemovesKnot1_0CorrectlyOneTime) {  // NOLINT
  bspline_1d_after_->RemoveKnot(ParamCoord(1), 0, 0.1);
  ASSERT_THAT(bspline_1d_after_->GetKnotVector(0)->GetNumberOfKnots(),
              bspline_1d_before_->GetKnotVector(0)->GetNumberOfKnots() - 1);
  ASSERT_THAT(bspline_1d_after_->GetKnotVector(0)->GetKnot(6).get(), DoubleEq(2));
  ASSERT_THAT(bspline_1d_after_->GetNumberOfControlPoints(), bspline_1d_before_->GetNumberOfControlPoints() - 1);
  std::vector<baf::ControlPoint> new_control_points = {
      baf::ControlPoint(std::vector<double>({0.0, 0.0})),
      baf::ControlPoint(std::vector<double>({0.0, 1.5})),
      baf::ControlPoint(std::vector<double>({1.0, 2.0})),
      baf::ControlPoint(std::vector<double>({3.0, 2.0})),
      baf::ControlPoint(std::vector<double>({4.0, 1.5})),
      baf::ControlPoint(std::vector<double>({4.0, 0.0}))
  };
  for (int i = 0; i < static_cast<int>(new_control_points.size()); ++i) {
    for (int j = 0; j < 2; ++j) {
      ASSERT_THAT(bspline_1d_after_->GetControlPoint({i}, j), DoubleEq(new_control_points[i].GetValue(j)));
    }
  }
  double s = 50;
  for (int i = 0; i <= s; ++i) {
    std::array<ParamCoord, 1> param_coord{ParamCoord(2 * i / s)};
    ASSERT_THAT(bspline_1d_after_->Evaluate(param_coord, {0})[0],
                DoubleEq(bspline_1d_before_->Evaluate(param_coord, {0})[0]));
    ASSERT_THAT(bspline_1d_after_->Evaluate(param_coord, {1})[0],
                DoubleEq(bspline_1d_before_->Evaluate(param_coord, {1})[0]));
  }
}

TEST_F(BSplineFig5_26, RemovesKnot1_0CorrectlyTwoTimes) {  // NOLINT
  bspline_1d_after_->RemoveKnot(ParamCoord(1), 0, 0.0, 2);
  ASSERT_THAT(bspline_1d_after_->GetKnotVector(0)->GetNumberOfKnots(),
              bspline_1d_before_->GetKnotVector(0)->GetNumberOfKnots() - 2);
  ASSERT_THAT(bspline_1d_after_->GetKnotVector(0)->GetKnot(5).get(), DoubleEq(2));
  ASSERT_THAT(bspline_1d_after_->GetNumberOfControlPoints(), bspline_1d_before_->GetNumberOfControlPoints() - 2);
  std::vector<baf::ControlPoint> new_control_points = {
      baf::ControlPoint(std::vector<double>({0.0, 0.0})),
      baf::ControlPoint(std::vector<double>({0.0, 1.5})),
      baf::ControlPoint(std::vector<double>({2.0, 2.5})),
      baf::ControlPoint(std::vector<double>({4.0, 1.5})),
      baf::ControlPoint(std::vector<double>({4.0, 0.0}))
  };
  for (int i = 0; i < static_cast<int>(new_control_points.size()); ++i) {
    for (int j = 0; j < 2; ++j) {
      ASSERT_THAT(bspline_1d_after_->GetControlPoint({i}, j), DoubleEq(new_control_points[i].GetValue(j)));
    }
  }
  double s = 50;
  for (int i = 0; i <= s; ++i) {
    std::array<ParamCoord, 1> param_coord{ParamCoord(2 * i / s)};
    ASSERT_THAT(bspline_1d_after_->Evaluate(param_coord, {0})[0],
                DoubleEq(bspline_1d_before_->Evaluate(param_coord, {0})[0]));
    ASSERT_THAT(bspline_1d_after_->Evaluate(param_coord, {1})[0],
                DoubleEq(bspline_1d_before_->Evaluate(param_coord, {1})[0]));
  }
}

TEST_F(BSplineFig5_26, RemovesKnot1_0CorrectlyThreeTimesAtOnce) {  // NOLINT
  ASSERT_THAT(bspline_1d_after_->RemoveKnot(ParamCoord(1), 0, 0.5, 3), 3);
  ASSERT_THAT(bspline_1d_after_->GetKnotVector(0)->GetNumberOfKnots(),
              bspline_1d_before_->GetKnotVector(0)->GetNumberOfKnots() - 3);
  ASSERT_THAT(bspline_1d_after_->GetKnotVector(0)->GetKnot(4).get(), DoubleEq(2));
  ASSERT_THAT(bspline_1d_after_->GetNumberOfControlPoints(), bspline_1d_before_->GetNumberOfControlPoints() - 3);
  std::vector<baf::ControlPoint> new_control_points = {
      baf::ControlPoint(std::vector<double>({0.0, 0.0})),
      baf::ControlPoint(std::vector<double>({0.0, 3.0})),
      baf::ControlPoint(std::vector<double>({4.0, 3.0})),
      baf::ControlPoint(std::vector<double>({4.0, 0.0}))
  };
  for (int i = 0; i < static_cast<int>(new_control_points.size()); ++i) {
    for (int j = 0; j < 2; ++j) {
      ASSERT_THAT(bspline_1d_after_->GetControlPoint({i}, j), DoubleEq(new_control_points[i].GetValue(j)));
    }
  }
  double s = 50;
  for (int i = 0; i <= s; ++i) {
    std::array<ParamCoord, 1> param_coord{ParamCoord(2 * i / s)};
    ASSERT_THAT(bspline_1d_after_->Evaluate(param_coord, {0})[0],
                DoubleEq(bspline_1d_before_->Evaluate(param_coord, {0})[0]));
    ASSERT_THAT(bspline_1d_after_->Evaluate(param_coord, {1})[0],
                DoubleNear(bspline_1d_before_->Evaluate(param_coord, {1})[0], 0.5));
  }
}

TEST_F(BSplineFig5_26, RemovesOnlyTwoKnots1_0CorrectlyWithTolerance0_1) {  // NOLINT
  ASSERT_THAT(bspline_1d_after_->RemoveKnot(ParamCoord(1), 0, 0.1, 3), 2);
}

class BSplineFig5_27 : public Test {  // NOLINT
 public:
  BSplineFig5_27() {
    std::array<Degree, 1> degree = {Degree{3}};
    KnotVectors<1> knot_vector_before = {std::make_shared<baf::KnotVector>(
        baf::KnotVector({ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0.3}, ParamCoord{0.5},
                         ParamCoord{0.5}, ParamCoord{0.5}, ParamCoord{0.7}, ParamCoord{0.7}, ParamCoord{1},
                         ParamCoord{1}, ParamCoord{1},
                         ParamCoord{1}}))};
    std::vector<baf::ControlPoint> control_points = {
        baf::ControlPoint(std::vector<double>({0.1, 0.0})),
        baf::ControlPoint(std::vector<double>({0.0, 1.0})),
        baf::ControlPoint(std::vector<double>({1.0, 2.0})),
        baf::ControlPoint(std::vector<double>({2.5, 2.25})),
        baf::ControlPoint(std::vector<double>({3.0, 2.15})),
        baf::ControlPoint(std::vector<double>({3.5, 2.0})),
        baf::ControlPoint(std::vector<double>({4.0, 1.8})),
        baf::ControlPoint(std::vector<double>({4.5, 1.0})),
        baf::ControlPoint(std::vector<double>({4.3, 0.0})),
        baf::ControlPoint(std::vector<double>({6.0, 0.0}))
    };
    bspline_1d_before_ = std::make_shared<spl::BSpline<1>>(knot_vector_before, degree, control_points);
    spl::BSpline<1> b_spline_after(*bspline_1d_before_);
    bspline_1d_after_ = std::make_shared<spl::BSpline<1>>(b_spline_after);
  }

 protected:
  std::shared_ptr<spl::BSpline<1>> bspline_1d_before_;
  std::shared_ptr<spl::BSpline<1>> bspline_1d_after_;
};

TEST_F(BSplineFig5_27, RemovesKnot0_3Correctly) {  // NOLINT
  bspline_1d_after_->RemoveKnot(ParamCoord(0.3), 0, 0.15);
  ASSERT_THAT(bspline_1d_after_->GetKnotVector(0)->GetNumberOfKnots(),
              bspline_1d_before_->GetKnotVector(0)->GetNumberOfKnots() - 1);
  ASSERT_THAT(bspline_1d_after_->GetKnotVector(0)->GetKnot(4).get(), DoubleEq(0.5));
  ASSERT_THAT(bspline_1d_after_->GetNumberOfControlPoints(), bspline_1d_before_->GetNumberOfControlPoints() - 1);
  std::vector<baf::ControlPoint> new_control_points = {
      baf::ControlPoint(std::vector<double>({0.1, 0.0})),
      baf::ControlPoint(std::vector<double>({-1.0 / 15.0, 5.0 / 3.0})),
      baf::ControlPoint(std::vector<double>({1.75, 2.4})),
      baf::ControlPoint(std::vector<double>({3.0, 2.15})),
      baf::ControlPoint(std::vector<double>({3.5, 2.0})),
      baf::ControlPoint(std::vector<double>({4.0, 1.8})),
      baf::ControlPoint(std::vector<double>({4.5, 1.0})),
      baf::ControlPoint(std::vector<double>({4.3, 0.0})),
      baf::ControlPoint(std::vector<double>({6.0, 0.0}))
  };
  for (int i = 0; i < static_cast<int>(new_control_points.size()); ++i) {
    for (int j = 0; j < 2; ++j) {
      ASSERT_THAT(bspline_1d_after_->GetControlPoint({i}, j), DoubleEq(new_control_points[i].GetValue(j)));
    }
  }
  double s = 50.0;
  for (int i = 0; i <= s; ++i) {
    std::array<ParamCoord, 1> param_coord{ParamCoord(i / s)};
    ASSERT_THAT(bspline_1d_after_->Evaluate(param_coord, {0})[0],
                DoubleNear(bspline_1d_before_->Evaluate(param_coord, {0})[0], 0.02));
    ASSERT_THAT(bspline_1d_after_->Evaluate(param_coord, {1})[0],
                DoubleNear(bspline_1d_before_->Evaluate(param_coord, {1})[0], 0.1));
  }
  std::any spline_before = std::make_any<std::shared_ptr<spl::BSpline<1>>>(bspline_1d_before_);
  std::any spline_after = std::make_any<std::shared_ptr<spl::BSpline<1>>>(bspline_1d_after_);
  io::IRITWriter iritWriter;
  iritWriter.WriteFile({spline_before, spline_after}, "bsplines.itd");
}

class NURBSFig5_26 : public Test {  // NOLINT
 public:
  NURBSFig5_26() {
    std::array<Degree, 1> degree = {Degree{3}};
    KnotVectors<1> knot_vector_before = {std::make_shared<baf::KnotVector>(
        baf::KnotVector({ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{1}, ParamCoord{1},
                         ParamCoord{1}, ParamCoord{2}, ParamCoord{2}, ParamCoord{2}, ParamCoord{2}}))};
    std::vector<baf::ControlPoint> control_points = {
        baf::ControlPoint(std::vector<double>({0.0, 0.0})),
        baf::ControlPoint(std::vector<double>({0.0, 1.5})),
        baf::ControlPoint(std::vector<double>({1.0, 2.0})),
        baf::ControlPoint(std::vector<double>({2.0, 2.0})),
        baf::ControlPoint(std::vector<double>({3.0, 2.0})),
        baf::ControlPoint(std::vector<double>({4.0, 1.5})),
        baf::ControlPoint(std::vector<double>({4.0, 0.0}))
    };
    std::vector<double> weights = {10, 0.5, 4, 2, 1, 0.5, 4};
    nurbs_1d_before_ = std::make_shared<spl::NURBS<1>>(knot_vector_before, degree, control_points, weights);
    spl::NURBS<1> nurbs_after(*nurbs_1d_before_);
    nurbs_1d_after_ = std::make_shared<spl::NURBS<1>>(nurbs_after);
  }

 protected:
  std::shared_ptr<spl::NURBS<1>> nurbs_1d_before_;
  std::shared_ptr<spl::NURBS<1>> nurbs_1d_after_;
};

TEST_F(NURBSFig5_26, RemovesKnot1_0CorrectlyOneTime) {  // NOLINT
  nurbs_1d_after_->RemoveKnot(ParamCoord(1), 0, 2);
  ASSERT_THAT(nurbs_1d_after_->GetKnotVector(0)->GetNumberOfKnots(),
              nurbs_1d_before_->GetKnotVector(0)->GetNumberOfKnots() - 1);
  ASSERT_THAT(nurbs_1d_after_->GetKnotVector(0)->GetKnot(6).get(), DoubleEq(2));
  ASSERT_THAT(nurbs_1d_after_->GetNumberOfControlPoints(), nurbs_1d_before_->GetNumberOfControlPoints() - 1);
  std::vector<baf::ControlPoint> new_control_points = {
      baf::ControlPoint(std::vector<double>({0.0, 0.0})),
      baf::ControlPoint(std::vector<double>({0.0, 1.5})),
      baf::ControlPoint(std::vector<double>({1.0, 2.0})),
      baf::ControlPoint(std::vector<double>({3.0, 2.0})),
      baf::ControlPoint(std::vector<double>({4.0, 1.5})),
      baf::ControlPoint(std::vector<double>({4.0, 0.0}))
  };
  std::vector<double> new_weights = {10, 0.5, 4, 1, 0.5, 4};
  for (int i = 0; i < static_cast<int>(new_control_points.size()); ++i) {
    for (int j = 0; j < 2; ++j) {
      ASSERT_THAT(nurbs_1d_after_->GetControlPoint({i}, j), DoubleEq(new_control_points[i].GetValue(j)));
    }
  }
  double s = 50;
  for (int i = 0; i <= s; ++i) {
    std::array<ParamCoord, 1> param_coord{ParamCoord(2 * i / s)};
    ASSERT_THAT(nurbs_1d_after_->Evaluate(param_coord, {0})[0],
                DoubleNear(nurbs_1d_before_->Evaluate(param_coord, {0})[0], 0.7));
    ASSERT_THAT(nurbs_1d_after_->Evaluate(param_coord, {1})[0],
                DoubleNear(nurbs_1d_before_->Evaluate(param_coord, {1})[0], 0.7));
  }
}

class NURBSFig5_27 : public Test {  // NOLINT
 public:
  NURBSFig5_27() {
    std::array<Degree, 1> degree = {Degree{3}};
    KnotVectors<1> knot_vector_before = {std::make_shared<baf::KnotVector>(
        baf::KnotVector({ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0.3}, ParamCoord{0.5},
                         ParamCoord{0.5}, ParamCoord{0.5}, ParamCoord{0.7}, ParamCoord{0.7}, ParamCoord{1},
                         ParamCoord{1}, ParamCoord{1},
                         ParamCoord{1}}))};
    std::vector<baf::ControlPoint> control_points = {
        baf::ControlPoint(std::vector<double>({0.1, 0.0})),
        baf::ControlPoint(std::vector<double>({0.0, 1.0})),
        baf::ControlPoint(std::vector<double>({1.0, 2.0})),
        baf::ControlPoint(std::vector<double>({2.5, 2.25})),
        baf::ControlPoint(std::vector<double>({3.0, 2.15})),
        baf::ControlPoint(std::vector<double>({3.5, 2.0})),
        baf::ControlPoint(std::vector<double>({4.0, 1.8})),
        baf::ControlPoint(std::vector<double>({4.5, 1.0})),
        baf::ControlPoint(std::vector<double>({4.3, 0.0})),
        baf::ControlPoint(std::vector<double>({6.0, 0.0}))
    };
    std::vector<double> weights = {1, 0.5, 2, 4, 1, 1, 2, 1, 1, 2};
    nurbs_1d_before_ = std::make_shared<spl::NURBS<1>>(knot_vector_before, degree, control_points, weights);
    spl::NURBS<1> nurbs_after(*nurbs_1d_before_);
    nurbs_1d_after_ = std::make_shared<spl::NURBS<1>>(nurbs_after);
  }

 protected:
  std::shared_ptr<spl::NURBS<1>> nurbs_1d_before_;
  std::shared_ptr<spl::NURBS<1>> nurbs_1d_after_;
};

TEST_F(NURBSFig5_27, RemovesKnot0_3Correctly) {  // NOLINT
  nurbs_1d_after_->RemoveKnot(ParamCoord(0.3), 0, 5);
  ASSERT_THAT(nurbs_1d_after_->GetKnotVector(0)->GetNumberOfKnots(),
              nurbs_1d_before_->GetKnotVector(0)->GetNumberOfKnots() - 1);
  ASSERT_THAT(nurbs_1d_after_->GetKnotVector(0)->GetKnot(4).get(), DoubleEq(0.5));
  ASSERT_THAT(nurbs_1d_after_->GetNumberOfControlPoints(), nurbs_1d_before_->GetNumberOfControlPoints() - 1);

  std::any spline_before = std::make_any<std::shared_ptr<spl::NURBS<1>>>(nurbs_1d_before_);
  std::any spline_after = std::make_any<std::shared_ptr<spl::NURBS<1>>>(nurbs_1d_after_);
  io::IRITWriter iritWriter;
  iritWriter.WriteFile({spline_before, spline_after}, "nurbs.itd");

  std::vector<baf::ControlPoint> new_control_points = {
      baf::ControlPoint(std::vector<double>({0.1, 0.0})),
      baf::ControlPoint(std::vector<double>({-0.4, 5.0})),
      baf::ControlPoint(std::vector<double>({41.0 / 17.0, 2313.0 / 1020.0})),
      baf::ControlPoint(std::vector<double>({3.0, 2.15})),
      baf::ControlPoint(std::vector<double>({3.5, 2.0})),
      baf::ControlPoint(std::vector<double>({4.0, 1.8})),
      baf::ControlPoint(std::vector<double>({4.5, 1.0})),
      baf::ControlPoint(std::vector<double>({4.3, 0.0})),
      baf::ControlPoint(std::vector<double>({6.0, 0.0}))
  };
  std::vector<double> new_weights = {1, 1.0 / 6.0, 8.5, 1, 1, 2, 1, 1, 2};
  for (int i = 0; i < static_cast<int>(new_control_points.size()); ++i) {
    for (int j = 0; j < 2; ++j) {
      ASSERT_THAT(nurbs_1d_after_->GetControlPoint({i}, j), DoubleEq(new_control_points[i].GetValue(j)));
    }
    ASSERT_THAT(nurbs_1d_after_->GetWeight({i}), DoubleEq(new_weights[i]));
  }
  double s = 50.0;
  for (int i = 0; i <= s; ++i) {
    std::array<ParamCoord, 1> param_coord{ParamCoord(i / s)};
//    ASSERT_THAT(nurbs_1d_after_->Evaluate(param_coord, {0})[0],
//                DoubleNear(nurbs_1d_before_->Evaluate(param_coord, {0})[0], 0.02));
//    ASSERT_THAT(nurbs_1d_after_->Evaluate(param_coord, {1})[0],
//                DoubleNear(nurbs_1d_before_->Evaluate(param_coord, {1})[0], 0.1));
    std::cout << "i = " << i << " --> u = 0: "
              << nurbs_1d_after_->Evaluate(param_coord, {0})[0] - nurbs_1d_before_->Evaluate(param_coord, {0})[0];
    std::cout << ", u = 1: "
              << nurbs_1d_after_->Evaluate(param_coord, {1})[0] - nurbs_1d_before_->Evaluate(param_coord, {1})[0]
              << std::endl;
  }
}

class BSpline2DFig5_28 : public Test {  // NOLINT
 public:
  BSpline2DFig5_28() {
    std::array<Degree, 2> degree = {Degree{3}, Degree{2}};
    KnotVectors<2> knot_vector_before = {
        std::make_shared<baf::KnotVector>(
            baf::KnotVector({ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0.3},
                             ParamCoord{0.3}, ParamCoord{0.3}, ParamCoord{0.7}, ParamCoord{1}, ParamCoord{1},
                             ParamCoord{1}, ParamCoord{1}})),
        std::make_shared<baf::KnotVector>(
            baf::KnotVector({ParamCoord{0}, ParamCoord{0}, ParamCoord{0}, ParamCoord{0.25}, ParamCoord{0.5},
                             ParamCoord{0.75}, ParamCoord{1}, ParamCoord{1}, ParamCoord{1}}))};
    std::vector<baf::ControlPoint> control_points = {
        baf::ControlPoint(std::vector<double>({0, 1, 4})), baf::ControlPoint(std::vector<double>({0, 1.5, 3.9})),
        baf::ControlPoint(std::vector<double>({0, 2, 3.8})), baf::ControlPoint(std::vector<double>({0, 2.5, 3.7})),
        baf::ControlPoint(std::vector<double>({0, 3, 3.6})), baf::ControlPoint(std::vector<double>({0, 4, 3.3})),
        baf::ControlPoint(std::vector<double>({0, 5, 3})), baf::ControlPoint(std::vector<double>({0, 5.5, 3})),

        baf::ControlPoint(std::vector<double>({1, 1, 4.5})), baf::ControlPoint(std::vector<double>({1, 1.5, 4.4})),
        baf::ControlPoint(std::vector<double>({1, 2, 4.3})), baf::ControlPoint(std::vector<double>({1, 2.5, 4.2})),
        baf::ControlPoint(std::vector<double>({1, 3, 4.1})), baf::ControlPoint(std::vector<double>({1, 4, 3.8})),
        baf::ControlPoint(std::vector<double>({1, 5, 3.5})), baf::ControlPoint(std::vector<double>({1, 5.5, 3.5})),

        baf::ControlPoint(std::vector<double>({2.5, 1, 3.5})), baf::ControlPoint(std::vector<double>({2.5, 1.5, 3.4})),
        baf::ControlPoint(std::vector<double>({2.5, 2, 3.3})), baf::ControlPoint(std::vector<double>({2.5, 2.5, 3.2})),
        baf::ControlPoint(std::vector<double>({2.5, 3, 3.1})), baf::ControlPoint(std::vector<double>({2.5, 4, 2.8})),
        baf::ControlPoint(std::vector<double>({2.5, 5, 2.5})), baf::ControlPoint(std::vector<double>({2.5, 5.5, 2.5})),

        baf::ControlPoint(std::vector<double>({3.5, 1, 2.5})), baf::ControlPoint(std::vector<double>({3.5, 1.5, 2.4})),
        baf::ControlPoint(std::vector<double>({3.5, 2, 2.3})), baf::ControlPoint(std::vector<double>({3.5, 2.5, 2.2})),
        baf::ControlPoint(std::vector<double>({3.5, 3, 2.1})), baf::ControlPoint(std::vector<double>({3.5, 4, 1.8})),
        baf::ControlPoint(std::vector<double>({3.5, 5, 1.5})), baf::ControlPoint(std::vector<double>({3.5, 5.5, 1.5})),

        baf::ControlPoint(std::vector<double>({5, 1, 2})), baf::ControlPoint(std::vector<double>({5, 1.5, 1.9})),
        baf::ControlPoint(std::vector<double>({5, 2, 1.8})), baf::ControlPoint(std::vector<double>({5, 2.5, 1.7})),
        baf::ControlPoint(std::vector<double>({5, 3, 1.6})), baf::ControlPoint(std::vector<double>({5, 4, 1.3})),
        baf::ControlPoint(std::vector<double>({5, 5, 1})), baf::ControlPoint(std::vector<double>({5, 5.5, 1})),

        baf::ControlPoint(std::vector<double>({6.5, 1, 2.5})), baf::ControlPoint(std::vector<double>({6.5, 1.5, 2.4})),
        baf::ControlPoint(std::vector<double>({6.5, 2, 2.3})), baf::ControlPoint(std::vector<double>({6.5, 2.5, 2.2})),
        baf::ControlPoint(std::vector<double>({6.5, 3, 2.1})), baf::ControlPoint(std::vector<double>({6.5, 4, 1.8})),
        baf::ControlPoint(std::vector<double>({6.5, 5, 1.5})), baf::ControlPoint(std::vector<double>({6.5, 5.5, 1.5})),
    };
    bspline_2d_before_ = std::make_shared<spl::BSpline<2>>(knot_vector_before, degree, control_points);
    spl::BSpline<2> b_spline_after(*bspline_2d_before_);
    bspline_2d_after_ = std::make_shared<spl::BSpline<2>>(b_spline_after);
  }

 protected:
  std::shared_ptr<spl::BSpline<2>> bspline_2d_before_;
  std::shared_ptr<spl::BSpline<2>> bspline_2d_after_;
};

TEST_F(BSpline2DFig5_28, RemovesKnot0_3CorrectlyOneTime) {  // NOLINT
  bspline_2d_after_->RemoveKnot(ParamCoord(0.3), 0, 0.5);
  std::any spline = std::make_any<std::shared_ptr<spl::BSpline<2>>>(bspline_2d_before_);
  std::any spline2 = std::make_any<std::shared_ptr<spl::BSpline<2>>>(bspline_2d_after_);
  io::IRITWriter writer;
  writer.WriteFile({spline, spline2}, "surface.itd");
  ASSERT_THAT(bspline_2d_after_->GetKnotVector(0)->GetNumberOfKnots(),
              bspline_2d_before_->GetKnotVector(0)->GetNumberOfKnots() - 1);
  ASSERT_THAT(bspline_2d_after_->GetKnotVector(0)->GetKnot(6).get(), DoubleEq(0.7));
  ASSERT_THAT(bspline_2d_after_->GetNumberOfControlPoints(), bspline_2d_before_->GetNumberOfControlPoints() - 6);
  double s = 50;
  for (int i = 0; i <= s; ++i) {
    for (int j = 0; j <= s; ++j) {
      std::array<ParamCoord, 2> param_coord{ParamCoord(i / s), ParamCoord(j / s)};
      for (int k = 0; k < bspline_2d_after_->GetDimension(); ++k) {
        ASSERT_THAT(bspline_2d_after_->Evaluate(param_coord, {k})[0],
                    DoubleNear(bspline_2d_before_->Evaluate(param_coord, {k})[0], 0.072));
      }
    }
  }
}