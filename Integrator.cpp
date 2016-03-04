#include "boost/program_options.hpp"
#include "FileReader.hpp"
#include "SlidingIntegrator.hpp"

namespace bpo = boost::program_options;

int main(int argc, char* argv[]){
 
  boost::filesystem::path targetPath, outputPath;
  CosmogenicHunter::Bounds<double> integrationBounds;
  
  bpo::options_description optionDescription("Integrator usage");
  optionDescription.add_options()
  ("help,h", "Display this help message")
  ("target", bpo::value<boost::filesystem::path>(&targetPath)->required(), "Path of the file containing the histogram to integrate and its associated covariance matrix")
  ("output,o", bpo::value<boost::filesystem::path>(&outputPath)->required(), "Output file where to save the sliding integral values and errors")
  ("integration-bounds,b", bpo::value<CosmogenicHunter::Bounds<double>>(&integrationBounds)->required(), "Bounds(':' separator) for the integration");  
  bpo::positional_options_description positionalOptions;//to use arguments without "--"
  positionalOptions.add("target", 1);
  
  bpo::variables_map arguments;
  try{
    
    bpo::store(bpo::command_line_parser(argc, argv).options(optionDescription).positional(positionalOptions).run(), arguments);
    
    if(arguments.count("help") || argc == 1){
      
      std::cout<<optionDescription<<std::endl;
      return 0;
      
    }
      
    bpo::notify(arguments);//the arguments are ready to be used
    
  }
  catch(bpo::error& error){
    
    std::cerr<<error.what()<<std::endl;
    return 1;
    
  }
  catch(std::invalid_argument& error){
    
    std::cerr<<"Error: "<<error.what()<<std::endl;
    return 1;
    
  }
  
  if(!boost::filesystem::is_regular_file(targetPath)){
    
    std::cerr<<"Error: "<<targetPath<<" is not a regular file"<<std::endl;
    return 1;
    
  }
  else{
    
    try{
      
      Integrator::FileReader fileReader;
      auto data = fileReader.read(targetPath);
      
      Integrator::SlidingIntegrator slidingIntegrator(integrationBounds);
      auto integratedData = slidingIntegrator.integrate(data);
      
      TFile outputFile(outputPath.c_str(), "recreate");
      integratedData.mean.Write("Integrated_Data");
      
    }
    catch(std::exception& error){
      
      std::cerr<<"Error: "<<error.what()<<std::endl;
      return 1;
      
    }
    
  }
  
}
