#ifndef INTEGRATOR_DATA_H
#define INTEGRATOR_DATA_H

#include <iostream>
#include <iomanip>
#include "Eigen/Core"

namespace Integrator{
  
  template <class T>
  struct Data{
    
    T mean;
    Eigen::MatrixXd covariance;
    
  };

  template <class T>
  std::ostream& operator<<(std::ostream& output, const Data<T>& data){
    
    output<<"Mean:\n";
    data.mean.SavePrimite(output);
    output<<"Covariance:\n"<<data.covariance;
    return output;
    
  }

}

#endif