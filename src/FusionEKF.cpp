#include "FusionEKF.h"
#include "Eigen/Dense"
#include "tools.h"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

/*
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // clang-format off
  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_laser_ << 0.0225, 0,
              0, 0.0225;

  R_radar_ = MatrixXd(3, 3);
  R_radar_ << 0.02, 0, 0,
              0, 0.02, 0,
              0, 0, 0.02;

  H_laser_ = MatrixXd(2, 4);
  H_laser_ << 1, 0, 0, 0,
              0, 1, 0, 0;

  ekf_.P_ = MatrixXd(4, 4);
  ekf_.P_ << 1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 1000, 0,
             0, 0, 0, 1000;

  ekf_.F_ = MatrixXd(4, 4);
  ekf_.F_ << 1, 0, 1, 0,
             0, 1, 0, 1,
             0, 0, 1, 0,
             0, 0, 0, 1;
  // clang-format on
}

/**
* Destructor.
*/
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {
  /*****************************************************************************
   *  Initialization
   ****************************************************************************/
  if (!is_initialized_) {
    // first measurement
    ekf_.x_ = VectorXd(4);
    ekf_.x_ << 0, 0, 0, 0;

    previous_timestamp_ = measurement_pack.timestamp_;

    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      float rho = measurement_pack.raw_measurements_[0];
      float phi = measurement_pack.raw_measurements_[1];
      if (!rho || !phi) {
        return;
      }
      ekf_.x_ << rho * cos(phi), rho * sin(phi), 0, 0;
    } else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      if (!measurement_pack.raw_measurements_[0] ||
          !measurement_pack.raw_measurements_[1]) {
        return;
      }
      ekf_.x_ << measurement_pack.raw_measurements_[0],
          measurement_pack.raw_measurements_[1], 0, 0;
    }

    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }

  /*****************************************************************************
   *  Prediction
   ****************************************************************************/

  float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;
  previous_timestamp_ = measurement_pack.timestamp_;

  ekf_.F_(0, 2) = dt;
  ekf_.F_(1, 3) = dt;

  ekf_.Q_ = MatrixXd(4, 4);

  // clang-format off
  ekf_.Q_ << (pow(dt,4)/4)*noise_ax_, 0, (pow(dt,3)/2)*noise_ax_, 0,
            0, (pow(dt,4)/4)*noise_ay_, 0, (pow(dt,3)/2)*noise_ay_,
            (pow(dt,3)/2)*noise_ax_, 0, pow(dt,2)*noise_ax_, 0,
            0,(pow(dt,3)/2)*noise_ay_, 0, pow(dt,2)*noise_ay_;
  // clang-format on

  ekf_.Predict();

  /*****************************************************************************
   *  Update
   ****************************************************************************/

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    ekf_.Hj_ = tools.CalculateJacobian(ekf_.x_);
    ekf_.R_ = R_radar_;
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);
  } else {
    ekf_.H_ = H_laser_;
    ekf_.R_ = R_laser_;
    ekf_.Update(measurement_pack.raw_measurements_);
  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}
