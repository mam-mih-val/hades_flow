#include <cmath>
#include "AnalysisTree/Cuts.hpp"
#include "AnalysisTree/SimpleCut.hpp"

namespace AnalysisTree{

Cuts* GetHadesEventCuts(const std::string& branch, const std::string& name="HadesGoodEvent") {
  Cuts* event_cuts = new Cuts(name);
  SimpleCut vtx_xy_cut{{{"vtx_x", "vtx_y"}}, [](std::vector<double> r){
                         return sqrt(r.at(0)*r.at(0) + r.at(1)*r.at(1)) < 3.0;
                       }};
  SimpleCut vtx_z_cut({branch, "vtx_z"}, -60.0, 0.0);
  SimpleCut vtx_chi2_cut({branch,"vtx_chi2"}, 0.5, 40);
  SimpleCut good_vertex_cut({branch,"good_vertex_cluster"}, 0.5, 1.5);
  SimpleCut good_vertex_candidate_cut({branch,"good_vertex_candidate"}, 0.5, 1.5);
  SimpleCut good_start_cut({branch,"good_start"}, 0.5, 1.5);
  SimpleCut no_pile_up_start_cut({branch,"no_pile_up_start"}, 0.5, 1.5);
  SimpleCut no_veto_cut({branch,"no_veto"}, 0.5, 1.5);
  SimpleCut good_start_veto_cut({branch,"good_start_veto"}, 0.5, 1.5);
  SimpleCut good_start_meta_cut({branch,"good_start_meta"}, 0.5, 1.5);
  event_cuts->AddCuts({
      vtx_xy_cut,
      vtx_z_cut,
      vtx_chi2_cut,
      good_vertex_cut,
      good_vertex_candidate_cut,
      good_start_cut,
      no_pile_up_start_cut,
      no_veto_cut,
      good_start_veto_cut,
      good_start_meta_cut
  } );
  return event_cuts;
}

Cuts* GetHadesTrackCuts(const std::string& branch, const std::string& name="HadesGoodVertexTrack") {
  Cuts* vertex_tracks_cuts = new Cuts(name);
  SimpleCut dca_xy_cut({branch,"dca_xy"}, -10, 10);
  SimpleCut dca_z_cut({branch,"dca_z"}, -10, 10);
  SimpleCut chi2_rk_cut({branch,"chi2"}, 0, 100);

  vertex_tracks_cuts->AddCuts({
      dca_xy_cut,
      dca_z_cut,
      chi2_rk_cut
  });
  return vertex_tracks_cuts;
};

Cuts* GetHadesMetaHitsCuts(const std::string& branch, const std::string& name="HadesGoodMetaHit") {
  Cuts* tof_cuts = new Cuts(name);
  SimpleCut meta_match_chi2_cut({branch,"match_quality"}, 0, 3.0);

  tof_cuts->AddCuts({meta_match_chi2_cut});
  return tof_cuts;
};
Cuts* GetHadesWallHitsCuts(const std::string& branch, const std::string& name="HadesGoodWallHit") {
  Cuts* wall_cuts = new Cuts(name);
  SimpleCut beta_cut({branch,"beta"}, 0, 1.0);
  SimpleCut ring_by_ring_cuts({{branch,"ring"}, {branch,"beta"}, {branch,"signal"}},
                              []( std::vector<double> vars ){
                                if(  1.0 <= vars.at(0) && vars.at(0) <= 5 )
                                  return vars.at(1) > 0.55
                                      && vars.at(2) > 80.0;
                                if( vars.at(0) >= 6.0 && vars.at(0) <= 7.0 )
                                  return vars.at(1) > 0.55
                                      && vars.at(2) > 85.0;
                                if(  8.0 <= vars.at(0) && vars.at(0) <= 10.0 )
                                  return vars.at(1) > 0.55
                                      && vars.at(2) > 88.0;
                                return false;
                              });

  wall_cuts->AddCuts({beta_cut, ring_by_ring_cuts});
  return wall_cuts;
};

}



