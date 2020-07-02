//
// Created by mikhail on 7/2/20.
//

#ifndef HADES_FLOW_SRC_TOOLS_H_
#define HADES_FLOW_SRC_TOOLS_H_

#include "acceptance_correction.h"
#include "centrality.h"
class Tools {
public:
  static Tools* Instance(){
    if( !instance_ )
      instance_ = new Tools;
    return instance_;
  }
  [[nodiscard]] Centrality* GetCentrality() const { return centrality_; }
  [[nodiscard]] AcceptanceCorrection* GetCorrections() const { return corrections_; }

private:
  static Tools* instance_;

public:
  Tools() : centrality_(new Centrality), corrections_(new AcceptanceCorrection){}
  virtual ~Tools() = default;

  Centrality* centrality_;
  AcceptanceCorrection* corrections_;
};

#endif // HADES_FLOW_SRC_TOOLS_H_
