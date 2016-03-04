#ifndef INTEGRATOR_FILE_READER_H
#define INTEGRATOR_FILE_READER_H

#include "boost/filesystem.hpp"
#include "TFile.h"
#include "TKey.h"
#include "TH1D.h"
#include "TMatrixF.h"
#include "TMatrixD.h"
#include "Data.hpp"

namespace Integrator{

  class FileReader{
    
    Data<TH1D> data;
    bool foundHistogram;
    bool foundCovariance;
    void readAsHist(TObject* obj);
    void readAsMatrix(TObject* obj);

  public:
    FileReader();
    Data<TH1D> read(const boost::filesystem::path& filePath);

  };

}

#endif