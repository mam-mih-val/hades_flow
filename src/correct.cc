#include <iostream>
#include <chrono>

#include <GlobalConfig.h>

#include <CorrectTaskManager.h>

#include "tools.h"
#include "hades_cuts.h"

int main(int argc, char **argv) {
  using namespace std;
  if(argc < 3){
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./correct filelist.txt path/to/efficiency.root" << std::endl;
    exit(EXIT_FAILURE);
  }
  const string file_list = argv[1];
  const string efficiency_path=argv[2];
  Tools::Instance()->GetCorrections()->ReadMaps(efficiency_path);

  const string event_header = "event_header";
  const string vtx_tracks = "mdc_vtx_tracks";
  const string sim_tracks = "sim_tracks";
  const string sim_event = "sim_header";

  const float beam_y = 0.74;  //TODO read from DataHeader
  // Configuration will be here
  Qn::Axis<double> pt_axis("pT", 16, 0.0, 1.6);
  Qn::Axis<double> rapidity_axis("rapidity", 15, -0.75f + beam_y, 0.75f + beam_y);

  AnalysisTree::Variable centrality("Centrality", event_header,
                                    {"selected_tof_rpc_hits"},
                                    [](const std::vector<double> &var){
                                      return Tools::Instance()->GetCentrality()->GetCentrality5pc(var.at(0));
                                    });
  auto* global_config = new Qn::GlobalConfig();
  global_config->AddEventVar(centrality);
  global_config->AddCorrectionAxis( {"Centrality", 8, 0.0, 40.0} );
  AnalysisTree::Variable reco_phi( "phi", {{vtx_tracks, "phi"}}, [](const std::vector<double>& vars){
    return vars.at(0);
  } );
  AnalysisTree::Variable ones( "Ones" );
  AnalysisTree::Variable efficiency( "eff", {
      {vtx_tracks, "pT"},
      {vtx_tracks, "rapidity"},
//      {event_header, "selected_tof_rpc_hits"},
  }, []( const std::vector<double>& var ){
//                                      int cent_class = (int) Tools::Instance()->GetCentrality()->GetCentralityClass5pc(var.at(2));
                                      double pt = var.at(0);
                                      double y = var.at(1);
                                      double eff = Tools::Instance()->GetCorrections()->GetEfficiency(0, pt, y);
                                      return 1.0/eff;
                                    } );
  Qn::QvectorTracksConfig un_reco("tracks_mdc", reco_phi, efficiency,
                                         {pt_axis, rapidity_axis});
  un_reco.SetCorrectionSteps(true, true, true);
  un_reco.AddCut( {AnalysisTree::Variable("mdc_vtx_tracks","geant_pid")}, [](double pid) { return abs(pid - 14.0) < 0.1; } );
  global_config->AddTrackQvector(un_reco);

  AnalysisTree::Variable sim_phi( "sim_tracks", "phi" );
  Qn::QvectorTracksConfig un_sim("sim_tracks", sim_phi, ones,
                                         {pt_axis, rapidity_axis});

  un_sim.SetCorrectionSteps(false, false, false);
  un_sim.AddCut({{"sim_tracks", "geant_pid"}},
                [](double pid) { return fabs(pid - 14.0) < 0.1; } );
  un_sim.AddCut( {{"sim_tracks", "is_primary"}},
                [](double flag) { return fabs(flag - 1.0) < 0.1; } );
  global_config->AddTrackQvector(un_sim);

  AnalysisTree::Variable reaction_plane(sim_event, "reaction_plane");

  Qn::QvectorConfig psi_rp("psi_rp", reaction_plane, ones);

  psi_rp.SetCorrectionSteps(false, false, false);
  global_config->SetPsiQvector(psi_rp);

 // ***********************************************
  // first filelist should contain DataHeader

  Qn::CorrectTaskManager task_manager({file_list}, {"hades_analysis_tree"});

  task_manager.AddBranchCut(AnalysisTree::GetHadesTrackCuts(vtx_tracks));
  task_manager.AddBranchCut(AnalysisTree::GetHadesEventCuts(event_header));

  auto* task = new Qn::CorrectionTask(global_config);
  task_manager.AddTask(task);
  task_manager.Init();
  auto start = std::chrono::system_clock::now();
  task_manager.Run(1000);
  task_manager.Finish();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
//  std::cout << ((double) n_events)*3.6 /elapsed_seconds.count() << "K events per hour\n";
  return 0;
}
