#include "SlidingIntegrator.hpp"

namespace Integrator{

  void SlidingIntegrator::findIndexesFor(const Data<TH1D>& data){
    
    minIndex = data.mean.FindFixBin(integrationBounds.getLowEdge()) - 1;//unsigned could end up being the max value if underflow (0) returned
    maxIndex = data.mean.FindFixBin(integrationBounds.getUpEdge()) - 1;
    if(minIndex < maxIndex) integrationSize = maxIndex - minIndex;
    else throw std::runtime_error("Overflow and underflow bins selected by the integration bounds");

  }

  std::vector<double> SlidingIntegrator::getIntegratedCentralValues(const TH1D& histogram){

    std::vector<double> integratedCentralValues(integrationSize);
    std::vector<double> centralValues(histogram.GetArray()+1+minIndex, histogram.GetArray()+1+maxIndex);
    
    std::partial_sum(centralValues.crbegin(), centralValues.crend(), integratedCentralValues.rbegin());//start from the end anf fill the last element with reverse iterators
    
    return integratedCentralValues;
    
  }
  
  Eigen::MatrixXd SlidingIntegrator::getIntegratedCovariance(const Eigen::MatrixXd& covariance){
    
    Eigen::MatrixXd integratedCovariance(integrationSize, integrationSize);

    for(unsigned i = 0; i != integrationSize; ++i)
      for(unsigned j = 0; j != integrationSize; ++j)
	integratedCovariance(i, j) = covariance.block(minIndex + i, minIndex + j, integrationSize - i, integrationSize - j).sum();
      
    return integratedCovariance;

  }
    
  SlidingIntegrator::SlidingIntegrator(CosmogenicHunter::Bounds<double> integrationBounds):integrationBounds(std::move(integrationBounds)){

  }

  Data<TGraphErrors> SlidingIntegrator::integrate(const Data<TH1D>& data){
    
    findIndexesFor(data);
    
    auto integratedCentralValues = getIntegratedCentralValues(data.mean);
    auto integratedCovariance = getIntegratedCovariance(data.covariance);
    
    std::vector<double> binLowEdge(integrationSize);
    binLowEdge.assign();
    data.mean.GetLowEdge();
    for(unsigned k = 0; k != binLowEdge.size();  ++k) binLowEdge.at(k) = data.mean.GetBinLowEdge(minIndex + 1 + k);
    Eigen::VectorXd integratedErrors = integratedCovariance.diagonal().array().sqrt();
    
    TGraphErrors integratedGraph(integrationSize, binLowEdge.data(), integratedCentralValues.data(), nullptr, integratedErrors.data());
    return Data<TGraphErrors>{std::move(integratedGraph), std::move(integratedCovariance)};

  }

}
