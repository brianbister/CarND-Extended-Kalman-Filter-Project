#include "tools.h"
#include <iostream>

using Eigen::MatrixXd;
using Eigen::VectorXd;

Tools::Tools() {}

Tools::~Tools() {}

VectorXd Tools::CalculateRMSE(const std::vector<VectorXd> &estimations,
                              const std::vector<VectorXd> &ground_truth) {
  /**
  TODO:
    * Calculate the RMSE here.
  */
}

MatrixXd Tools::CalculateJacobian(const VectorXd &x_state) {
  /**
  TODO:
    * Calculate a Jacobian here.
  */
}
