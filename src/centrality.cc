#include "centrality.h"

void Centrality::LoadCentralityAuAu123(){
  Double_t xAxis[17] = {0, 10, 17, 23, 29, 36, 45, 55, 68, 82, 99, 117, 136, 157, 180, 312, 499};
  au123_centrality_5pc_ = new TH1F("TOFRPC_5pc_fixedCuts__1","TOFRPC_5pc_fixedCuts",16, xAxis);
  au123_centrality_5pc_->SetBinContent(1,15);
  au123_centrality_5pc_->SetBinContent(2,14);
  au123_centrality_5pc_->SetBinContent(3,13);
  au123_centrality_5pc_->SetBinContent(4,12);
  au123_centrality_5pc_->SetBinContent(5,11);
  au123_centrality_5pc_->SetBinContent(6,10);
  au123_centrality_5pc_->SetBinContent(7,9);
  au123_centrality_5pc_->SetBinContent(8,8);
  au123_centrality_5pc_->SetBinContent(9,7);
  au123_centrality_5pc_->SetBinContent(10,6);
  au123_centrality_5pc_->SetBinContent(11,5);
  au123_centrality_5pc_->SetBinContent(12,4);
  au123_centrality_5pc_->SetBinContent(13,3);
  au123_centrality_5pc_->SetBinContent(14,2);
  au123_centrality_5pc_->SetBinContent(15,1);
  au123_centrality_5pc_->SetBinError(1,1.8301);
  au123_centrality_5pc_->SetBinError(2,5.29551);
  au123_centrality_5pc_->SetBinError(3,5.07632);
  au123_centrality_5pc_->SetBinError(4,4.68075);
  au123_centrality_5pc_->SetBinError(5,4.83154);
  au123_centrality_5pc_->SetBinError(6,5.30302);
  au123_centrality_5pc_->SetBinError(7,4.84388);
  au123_centrality_5pc_->SetBinError(8,5.20623);
  au123_centrality_5pc_->SetBinError(9,4.82995);
  au123_centrality_5pc_->SetBinError(10,5.22526);
  au123_centrality_5pc_->SetBinError(11,4.99358);
  au123_centrality_5pc_->SetBinError(12,4.83847);
  au123_centrality_5pc_->SetBinError(13,4.94002);
  au123_centrality_5pc_->SetBinError(14,5.02256);
  au123_centrality_5pc_->SetBinError(15,5.08179);
  au123_centrality_5pc_->SetBinError(16,0.00100254);
  au123_centrality_5pc_->SetEntries(16);
};
void Centrality::LoadCentralityAgAg123(){
  Double_t xAxis1[14] = {0, 6, 10, 14, 19, 24, 31, 39, 50, 63, 78, 95, 164, 499};
  ag123_centrality_5pc_ = new TH1F("ag123_centrality_5pc_","TOFRPC_5pc_fixedCuts",13, xAxis1);
  ag123_centrality_5pc_->SetBinContent(1,12);
  ag123_centrality_5pc_->SetBinContent(2,11);
  ag123_centrality_5pc_->SetBinContent(3,10);
  ag123_centrality_5pc_->SetBinContent(4,9);
  ag123_centrality_5pc_->SetBinContent(5,8);
  ag123_centrality_5pc_->SetBinContent(6,7);
  ag123_centrality_5pc_->SetBinContent(7,6);
  ag123_centrality_5pc_->SetBinContent(8,5);
  ag123_centrality_5pc_->SetBinContent(9,4);
  ag123_centrality_5pc_->SetBinContent(10,3);
  ag123_centrality_5pc_->SetBinContent(11,2);
  ag123_centrality_5pc_->SetBinContent(12,1);
  ag123_centrality_5pc_->SetBinError(1,5.4316);
  ag123_centrality_5pc_->SetBinError(2,5.0324);
  ag123_centrality_5pc_->SetBinError(3,4.75178);
  ag123_centrality_5pc_->SetBinError(4,5.22188);
  ag123_centrality_5pc_->SetBinError(5,4.49145);
  ag123_centrality_5pc_->SetBinError(6,5.26705);
  ag123_centrality_5pc_->SetBinError(7,4.82019);
  ag123_centrality_5pc_->SetBinError(8,5.16245);
  ag123_centrality_5pc_->SetBinError(9,4.95292);
  ag123_centrality_5pc_->SetBinError(10,4.91232);
  ag123_centrality_5pc_->SetBinError(11,4.87075);
  ag123_centrality_5pc_->SetBinError(12,5.08416);
  ag123_centrality_5pc_->SetBinError(13,0.00102813);
  ag123_centrality_5pc_->SetEntries(13);
}
void Centrality::LoadCentralityAgAg158(){
  std::cout << "Centrality::LoadCentralityAgAg158() is not implemented yet" << std::endl;
  abort();
}

double Centrality::GetCentralityClass( double multiplicity, DATA data_type ){
  switch (data_type) {
  case DATA::AuAu_1_23AGeV :
    return GetAuAu123CentralityClass(multiplicity);
    break;
  case DATA::AgAg_1_23AGeV :
    return GetAgAg123CentralityClass(multiplicity);
    break;
  case DATA::AgAg_1_58AGeV :
    return GetAgAg158CentralityClass(multiplicity);
    break;
  default:
    std::cout << "Error in Centrality::GetCentralityClass()" << std::endl;
    std::cout << "No such data type" << std::endl;
    abort();
  }
}
double Centrality::GetCentrality( double multiplicity, DATA data_type ){
  switch (data_type) {
  case DATA::AuAu_1_23AGeV :
    return GetAuAu123Centrality(multiplicity);
    break;
  case DATA::AgAg_1_23AGeV :
    return GetAgAg123Centrality(multiplicity);
    break;
  case DATA::AgAg_1_58AGeV :
    return GetAgAg158Centrality(multiplicity);
    break;
  default:
    std::cout << "Error in Centrality::GetCentrality()" << std::endl;
    std::cout << "No such data type" << std::endl;
    abort();
  }
}

double Centrality::GetAuAu123CentralityClass(double multiplicity){
  if( !au123_centrality_5pc_ )
    LoadCentralityAuAu123();
  auto bin = au123_centrality_5pc_->FindBin(multiplicity);
  return au123_centrality_5pc_->GetBinContent(bin) - 1.0;
};
double Centrality::GetAuAu123Centrality(double multiplicity){
  return 5.0* GetAuAu123CentralityClass(multiplicity)+2.5;
}

double Centrality::GetAgAg123CentralityClass(double multiplicity){
  if( !ag123_centrality_5pc_ )
    LoadCentralityAgAg123();
  auto bin = ag123_centrality_5pc_->FindBin(multiplicity);
  return ag123_centrality_5pc_->GetBinContent(bin) - 1.0;
};
double Centrality::GetAgAg123Centrality(double multiplicity){
  return 5.0* GetAgAg123CentralityClass(multiplicity)+2.5;
}

double Centrality::GetAgAg158CentralityClass(double multiplicity){
  if( !ag158_centrality_5pc_ )
    LoadCentralityAgAg158();
  auto bin = ag158_centrality_5pc_->FindBin(multiplicity);
  return ag158_centrality_5pc_->GetBinContent(bin) - 1.0;
};
double Centrality::GetAgAg158Centrality(double multiplicity){
  return 5.0* GetAgAg158CentralityClass(multiplicity)+2.5;
}