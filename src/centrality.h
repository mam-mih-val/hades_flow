
#ifndef CENTRALITY_H
#define CENTRALITY_H

#include <iostream>
#include <string>

#include "TH1F.h"

class Centrality
{
public:
  enum class DATA{
    AuAu_1_23AGeV,
    AgAg_1_23AGeV,
    AgAg_1_58AGeV,
  };

  Centrality() : au123_centrality_5pc_{nullptr},
                 ag123_centrality_5pc_{nullptr},
                 ag158_centrality_5pc_{nullptr}{};
  ~Centrality() = default;

  [[maybe_unused]] double GetCentralityClass( double multiplicity, DATA data_type );
  [[maybe_unused]] double GetCentrality( double multiplicity, DATA data_type );

private:
  [[maybe_unused]] void LoadCentralityAuAu123();
  [[maybe_unused]] void LoadCentralityAgAg123();
  [[maybe_unused]] void LoadCentralityAgAg158();

  [[maybe_unused]] double inline GetAuAu123CentralityClass(double multiplicity);
  [[maybe_unused]] double inline GetAuAu123Centrality(double multiplicity);
  [[maybe_unused]] double inline GetAgAg123CentralityClass(double multiplicity);
  [[maybe_unused]] double inline GetAgAg123Centrality(double multiplicity);
  [[maybe_unused]] double inline GetAgAg158CentralityClass(double multiplicity);
  [[maybe_unused]] double inline GetAgAg158Centrality(double multiplicity);

  TH1F* au123_centrality_5pc_; // Centrality for apr 12 data of Au+Au@1.23AGeV
  TH1F* ag123_centrality_5pc_; // Centrality for mar 19 data of Ag+Ag@1.23AGeV
  TH1F* ag158_centrality_5pc_; // Centrality for mar 19 data of Ag+Ag@1.58AGeV

};

#endif // CENTRALITY_H