#ifndef INTEGRATOR_SLIDING_INTEGRATOR_H
#define INTEGRATOR_SLIDING_INTEGRATOR_H

#include "TH1D.h"
#include "TGraphErrors.h"
#include "Cosmogenic/Bounds.hpp"
#include "Data.hpp"

namespace Integrator{
  
  class SlidingIntegrator{
    
    CosmogenicHunter::Bounds<double> integrationBounds;
    unsigned minIndex, maxIndex, integrationSize;
    
    void findIndexesFor(const Data<TH1D>& data);
    std::vector<double> getIntegratedCentralValues(const TH1D& histogram);
    Eigen::MatrixXd getIntegratedCovariance(const Eigen::MatrixXd& covariance);

  public:
    SlidingIntegrator(CosmogenicHunter::Bounds<double> integrationBounds);
    Data<TGraphErrors> integrate(const Data<TH1D>& data);

  };

}

#endif