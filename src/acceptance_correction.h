//
// Created by mikhail on 7/2/20.
//

#ifndef HADES_FLOW_SRC_ACCEPTANCE_CORRECTION_H_
#define HADES_FLOW_SRC_ACCEPTANCE_CORRECTION_H_

#include <TFile.h>
#include <TH2F.h>
#include <iostream>

class AcceptanceCorrection {
public:
  AcceptanceCorrection() = default;
  virtual ~AcceptanceCorrection() = default;
  double GetEfficiency( int cent_class, double pt, double y ){
    try {
      int bin = efficiency_maps_.at(cent_class).FindBin(pt, y);
      return efficiency_maps_.at(cent_class).GetBinContent(bin);
    } catch (const std::exception& exception) {
      std::cout << "Error in AcceptanceCorrection::GetEfficiency()" << std::endl;
      std::cout << "Efficiency maps are not initialized" << std::endl;
      std::cout << exception.what() << std::endl;
      abort();
    }
  }
  double GetMismatch( int cent_class, double pt, double y ){
    try {
      int bin = mismatch_maps_.at(cent_class).FindBin(pt, y);
      return mismatch_maps_.at(cent_class).GetBinContent(bin);
    }catch (const std::exception& exception) {
      std::cout << "Error in AcceptanceCorrection::GetMismatch()" << std::endl;
      std::cout << "Mismatch maps are not initialized" << std::endl;
      std::cout << exception.what() << std::endl;
      abort();
    }
  }
  void ReadMaps(const std::string& file_name){
    auto* file = TFile::Open(file_name.c_str());
    if( !file ){
      std::cout << "No param file for acceptance correction" << std::endl;
      abort();
    }
    TH2F* histo{nullptr};
    int percentile=2;
    while ( percentile < 40 ){
      std::string name = "efficiency_prim_"+std::to_string(percentile);
      file->GetObject(name.c_str(), histo);
      if( !histo ){
        std::cout << "No such obj: " << name << std::endl;
        abort();
      }
      efficiency_maps_.emplace_back( *histo );
      name = "pid_mismatch_prim_" + std::to_string(percentile);
      file->GetObject(name.c_str(), histo);
      if( !histo ){
        std::cout << "No such obj: " << name << std::endl;
        abort();
      }
      mismatch_maps_.emplace_back(*histo);
      percentile+=5;
    }
  }
private:

  std::vector<TH2F> efficiency_maps_;
  std::vector<TH2F> mismatch_maps_;
};

#endif // HADES_FLOW_SRC_ACCEPTANCE_CORRECTION_H_
