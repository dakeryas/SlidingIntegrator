#include "FileReader.hpp"

namespace Integrator{

  bool itemMatches(TObject* obj, const std::string& className){

    return std::string(obj->ClassName()).find(className) != std::string::npos;
    
  }
  
  FileReader::FileReader():foundHistogram(false),foundCovariance(false){
    
  }

  void FileReader::readAsHist(TObject* obj){

    if(itemMatches(obj, "TH1D")){
      
      data.mean = *dynamic_cast<TH1D*>(obj);
      
    }
    else if(itemMatches(obj, "TH1F")){
      
      TH1D htemp;
      dynamic_cast<TH1F*>(obj)->Copy(htemp);//copy the TH1F into a TH1D
      data.mean = htemp;
      
    }
    
    foundHistogram = true;

  }

  void FileReader::readAsMatrix(TObject* obj){//reads a key to retrieve a TH1 and store it in h

    TMatrixD mTemp;
    if(itemMatches(obj, "TMatrixT<double>")){
      
      mTemp.ResizeTo(*dynamic_cast<TMatrixD*>(obj));
      mTemp = *dynamic_cast<TMatrixD*>(obj);//ROOT doesn't support direct equality between matrices of different sizes, and 'Copy' is broken for matrices with different sizes
      
    }
    else if(itemMatches(obj, "TMatrixT<float>")){
      
      mTemp.ResizeTo(*dynamic_cast<TMatrixF*>(obj));
      dynamic_cast<TMatrixF*>(obj)->Copy(mTemp);//copy the TMatrixF into a TMatrixD
      
    }
    
    data.covariance = Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(mTemp.GetMatrixArray(), mTemp.GetNrows(), mTemp.GetNcols());
    foundCovariance = true;

  }

  Data<TH1D> FileReader::read(const boost::filesystem::path& filePath){
    
    TFile file(filePath.c_str());
    TIter keyit(file.GetListOfKeys());

    TKey* currentKey;
    while((currentKey = dynamic_cast<TKey*>(keyit()))){
      
      auto readObject = currentKey->ReadObj();//this is silly to cast a TObject to a TKey to eventually return a TObject with the ReadObj method, but heh, this is ROOT !
      if(itemMatches(readObject, "TH1")){
	
	if(!foundHistogram) readAsHist(readObject);
	else throw std::runtime_error("There is more than one histogram in "+filePath.string());
	
      }
      else if(itemMatches(readObject, "TMatrix")){
	
	if(!foundCovariance) readAsMatrix(readObject);
	else throw std::runtime_error("There is more than one covariance matrix in "+filePath.string());
	
      }
      
    }
    
    if(foundHistogram && foundCovariance) return data;
    else throw std::runtime_error("Could not find a histogram and a matrix in "+filePath.string());
    
  }

}

