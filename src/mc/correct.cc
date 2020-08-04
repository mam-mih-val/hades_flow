#include <iostream>
#include <chrono>

#include <GlobalConfig.h>

#include <CorrectTaskManager.h>
#include <QnTools/Stats.hpp>

#include <AnalysisTree/Variable.hpp>

#include "tools.h"
#include "hades_cuts.h"

int main(int argc, char **argv) {
  using namespace std;
  if(argc < 1){
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./correct filelist.txt path/to/efficiency.root" << std::endl;
    exit(EXIT_FAILURE);
  }
  const string file_list = argv[1];

  const string event_header = "event_header";
  const string vtx_tracks = "mdc_vtx_tracks";
  const string sim_tracks = "sim_tracks";
  const string sim_event = "sim_header";
  const string wall_hits = "forward_wall_hits";

  const float beam_y = 0.74;  //TODO read from DataHeader
  // Configuration will be here
  Qn::Axis<double> pt_axis("pT", 16, 0.0, 1.6);
  Qn::Axis<double> rapidity_axis("rapidity", 15, -0.75 + beam_y, 0.75 + beam_y);


  AnalysisTree::Variable centrality("Centrality",
                                    {{event_header, "selected_tof_rpc_hits"}},
                                    [](const std::vector<double> &var){
                                      return Tools::Instance()->GetCentrality()->GetCentrality(
            var.at(0), Centrality::DATA::AuAu_1_23AGeV);});

  auto* global_config = new Qn::GlobalConfig();
  global_config->AddEventVar(centrality);
  global_config->AddCorrectionAxis( {"Centrality", 8, 0.0, 40.0} );
  // un-vector from MDC
  Qn::QvectorTracksConfig reco_vector("reco",
                                  {vtx_tracks, "phi"}, {"Ones"},
                                  {pt_axis, rapidity_axis});
  reco_vector.SetCorrectionSteps(false, false, false);
  reco_vector.AddCut( {AnalysisTree::Variable(vtx_tracks,"geant_pid")},
                 [](double pid) { return abs(pid - 14.0) < 0.1; } );
  reco_vector.AddCut( {AnalysisTree::Variable(sim_tracks,"is_primary")},
                 [](double flag) { return abs(flag - 1.0) < 0.1; } );
  reco_vector.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(reco_vector);

  Qn::QvectorTracksConfig sim_vector("sim",
                                  {sim_tracks, "phi"}, {"Ones"},
                                  {pt_axis, rapidity_axis});
  sim_vector.SetCorrectionSteps(false, false, false);
  sim_vector.AddCut( {AnalysisTree::Variable(sim_tracks,"geant_pid")},
                 [](double pid) { return abs(pid - 14.0) < 0.1; } );
  sim_vector.AddCut( {AnalysisTree::Variable(sim_tracks,"is_primary")},
                      [](double flag) { return abs(flag - 1.0) < 0.1; } );
  sim_vector.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(sim_vector);

  Qn::QvectorConfig psi_rp("psi_rp", {sim_event, "reaction_plane"}, {"Ones"});
  psi_rp.SetCorrectionSteps(false, false, false);
  global_config->SetPsiQvector(psi_rp);
 // ***********************************************
  // first filelist should contain DataHeader

  Qn::CorrectTaskManager task_manager({file_list}, {"hades_analysis_tree"});
  auto* task = new Qn::CorrectionTask(global_config);

  task->AddQAHistogram("reco", {{vtx_tracks + "_rapidity", 200, -0.75+beam_y, 0.75+beam_y},
                           {vtx_tracks + "_pT", 200, 0.0, 2.0}});
  task->AddQAHistogram("reco", {{vtx_tracks + "_pT", 200, 0.0, 2.0},
                             {vtx_tracks + "_phi", 315, -3.15, 3.15}});
  task->AddQAHistogram("reco", {{vtx_tracks + "_rapidity", 100, -0.75+beam_y, 0.75+beam_y},
                           {vtx_tracks + "_phi", 315, -3.15, 3.15}});

  task->AddQAHistogram("sim", {{sim_tracks + "_rapidity", 200, -0.75+beam_y, 0.75+beam_y},
                           {vtx_tracks + "_pT", 200, 0.0, 2.0}});
  task->AddQAHistogram("sim", {{sim_tracks + "_pT", 200, 0.0, 2.0},
                             {vtx_tracks + "_phi", 315, -3.15, 3.15}});
  task->AddQAHistogram("sim", {{sim_tracks + "_rapidity", 100, -0.75+beam_y, 0.75+beam_y},
                           {vtx_tracks + "_phi", 315, -3.15, 3.15}});

//  task_manager.AddBranchCut(AnalysisTree::GetHadesTrackCuts(vtx_tracks));
//  task_manager.AddBranchCut(AnalysisTree::GetHadesEventCuts(event_header));

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
