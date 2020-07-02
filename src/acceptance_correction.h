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
  AcceptanceCorrection(){
      Init();
  };
  virtual ~AcceptanceCorrection() = default;
  double GetEfficiency( int cent_class, double pt, double y ){
    int bin = efficiency_maps_.at(cent_class).FindBin(pt, y);
    return efficiency_maps_.at(cent_class).GetBinContent(bin);
  }
  double GetMismatch( int cent_class, double pt, double y ){
    int bin = mismatch_maps_.at(cent_class).FindBin(pt, y);
    return mismatch_maps_.at(cent_class).GetBinContent(bin);
  }
private:
  void Init(){
    auto* file = TFile::Open("../src/param/efficiency.root");
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
  std::vector<TH2F> efficiency_maps_;
  std::vector<TH2F> mismatch_maps_;
};

#endif // HADES_FLOW_SRC_ACCEPTANCE_CORRECTION_H_
