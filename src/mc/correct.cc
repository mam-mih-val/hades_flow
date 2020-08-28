#include <iostream>

#include <GlobalConfig.h>

#include <CorrectTaskManager.h>
#include <QnTools/Stats.hpp>

#include <AnalysisTree/Variable.hpp>

#include <centrality.h>
#include <cuts.h>

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

  AnalysisTree::Variable centrality("Centrality",
                                    {{event_header, "selected_tof_rpc_hits"}},
                                    [](const std::vector<double> &var){
                                      return HadesUtils::Centrality::GetValue(var.at(0),
                                                                              HadesUtils::DATA_TYPE::AuAu_1_23AGeV);});
  double beam_rapidity;
  try {
    beam_rapidity =
        AnalysisTree::GetObjectFromFileList<AnalysisTree::DataHeader>(
            file_list, "DataHeader")->GetBeamRapidity();
  } catch (const std::runtime_error& e) {
    beam_rapidity =
        AnalysisTree::GetObjectFromFileList<AnalysisTree::DataHeader>(
            file_list, "data_information")->GetBeamRapidity();
  }

  AnalysisTree::Variable y_cm("y_cm",
                              {{vtx_tracks, "rapidity"}},
                              [beam_rapidity](const std::vector<double> &var){
                                return var.at(0)-beam_rapidity;
                              });

  AnalysisTree::Variable y_cm_gen("y_cm_gen",
                              {{sim_tracks, "rapidity"}},
                              [beam_rapidity](const std::vector<double> &var){
                                return var.at(0)-beam_rapidity;
                              });

  Qn::AxisConfig pt_axis({vtx_tracks, "pT"}, 16, 0.0, 1.6);
  Qn::AxisConfig pt_axis_gen({sim_tracks, "pT"}, 16, 0.0, 1.6);
  Qn::AxisConfig rapidity_axis(y_cm, 15, -0.75, 0.75);
  Qn::AxisConfig rapidity_axis_gen(y_cm_gen, 15, -0.75, 0.75);

  auto* global_config = new Qn::GlobalConfig();
  global_config->AddEventVar(centrality);
  global_config->AddCorrectionAxis( {"Centrality", 8, 0.0, 40.0} );
  // un-vector from MDC
  Qn::QvectorTracksConfig pdg_prim("PDG_Prim",
                                  {vtx_tracks, "phi"}, {"Ones"},
                                  {pt_axis_gen, rapidity_axis_gen});
  pdg_prim.SetCorrectionSteps(true, false, false);
  pdg_prim.AddCut( {{vtx_tracks, "geant_pid"},
                       [](double pid) { return abs(pid - 14.0) < 0.1; }, "PDG-Prim, reco-pid"});
  pdg_prim.AddCut( {{sim_tracks, "geant_pid"},
                    [](double pid) { return abs(pid - 14.0) < 0.1; }, "PDG-Prim, sim-pid"});
  pdg_prim.AddCut( {{sim_tracks, "is_primary"},
                    [](double pid) { return abs(pid - 1.0) < 0.1; }, "PDG-Prim, cut on primary"});
  pdg_prim.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(pdg_prim);

  Qn::QvectorTracksConfig pdg_sec("PDG_Sec",
                                  {vtx_tracks, "phi"}, {"Ones"},
                                  {pt_axis_gen, rapidity_axis_gen});
  pdg_sec.SetCorrectionSteps(true, false, false);
  pdg_sec.AddCut( {{vtx_tracks, "geant_pid"},
                       [](double pid) { return abs(pid - 14.0) < 0.1; }, "PDG_Sec, cut on proton reco-pid"});
  pdg_sec.AddCut( {{sim_tracks, "geant_pid"},
                    [](double pid) { return abs(pid - 14.0) < 0.1; }, "PDG_Sec, cut on proton sim-pid"});
  pdg_sec.AddCut( {{sim_tracks, "is_primary"},
                    [](double pid) { return abs(pid - 0.0) < 0.1; }, "PDG_Sec, cut on not primary"});
  pdg_sec.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(pdg_sec);

  Qn::QvectorTracksConfig pid_prim("PID_Prim",
                                  {vtx_tracks, "phi"}, {"Ones"},
                                  {pt_axis, rapidity_axis});
  pid_prim.SetCorrectionSteps(true, false, false);
  pid_prim.AddCut( {AnalysisTree::Variable(vtx_tracks, "geant_pid"),
                       [](double pid) { return abs(pid - 14.0) < 0.1; }, "PID_Prim, cut on proton reco-pid"});
  pid_prim.AddCut( {AnalysisTree::Variable(sim_tracks, "is_primary"),
                    [](double pid) { return abs(pid - 1.0) < 0.1; }, "PID_Prim, cut on primary"});
  pid_prim.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(pid_prim);

  Qn::QvectorTracksConfig pid_sec("PID_Sec",
                                  {vtx_tracks, "phi"}, {"Ones"},
                                  {pt_axis, rapidity_axis});
  pid_sec.SetCorrectionSteps(true, false, false);
  pid_sec.AddCut( {AnalysisTree::Variable(vtx_tracks, "geant_pid"),
                       [](double pid) { return abs(pid - 14.0) < 0.1; }, "PID_Sec, cut on proton reco-pid"});
  pid_sec.AddCut( {AnalysisTree::Variable(sim_tracks, "is_primary"),
                    [](double pid) { return abs(pid - 0.0) < 0.1; }, "PID_Sec, cut on not primary"});
  pid_sec.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(pid_sec);

  Qn::QvectorTracksConfig pid_reco("PID_Reco",
                                  {vtx_tracks, "phi"}, {"Ones"},
                                  {pt_axis, rapidity_axis});
  pid_reco.SetCorrectionSteps(true, false, false);
  pid_reco.AddCut( {AnalysisTree::Variable(vtx_tracks, "geant_pid"),
                       [](double pid) { return abs(pid - 14.0) < 0.1; }, "PID_Reco, cut on proton reco-pid"});
  pid_reco.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(pid_reco);

  Qn::QvectorTracksConfig gen_prim("GEN_Prim",
                                  {sim_tracks, "phi"}, {"Ones"},
                                  {pt_axis_gen, rapidity_axis_gen});
  gen_prim.SetCorrectionSteps(false, false, false);
  gen_prim.AddCut({AnalysisTree::Variable(sim_tracks, "geant_pid"),
                   [](double pid) { return abs(pid - 14.0) < 0.1; }, "GEN_Prim, cut on proton sim-pid"});
  gen_prim.AddCut( {AnalysisTree::Variable(sim_tracks, "is_primary"),
                    [](double pid) { return abs(pid - 1.0) < 0.1; }, "GEN_Prim, cut on primary"} );
  gen_prim.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(gen_prim);

  Qn::QvectorTracksConfig gen_sec("GEN_Sec",
                                  {sim_tracks, "phi"}, {"Ones"},
                                  {pt_axis_gen, rapidity_axis_gen});
  gen_sec.SetCorrectionSteps(false, false, false);
  gen_sec.AddCut({AnalysisTree::Variable(sim_tracks, "geant_pid"),
                   [](double pid) { return abs(pid - 14.0) < 0.1; }, "GEN_Sec, cut on proton sim-pid"});
  gen_sec.AddCut( {AnalysisTree::Variable(sim_tracks, "is_primary"),
                    [](double pid) { return abs(pid - 0.0) < 0.1; }, "GEN_Sec, cut on is not primary"} );
  gen_sec.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(gen_sec);

  Qn::QvectorConfig psi_rp("psi_rp", {sim_event, "reaction_plane"}, {"Ones"});
  psi_rp.SetCorrectionSteps(false, false, false);
  global_config->SetPsiQvector(psi_rp);
 // ***********************************************
  // first filelist should contain DataHeader

  Qn::CorrectTaskManager task_manager({file_list}, {"hades_analysis_tree"});
  auto* task = new Qn::CorrectionTask(global_config);

  task_manager.SetEventCuts(HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::EVENT_HEADER,
                                                  HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
  task_manager.AddBranchCut(HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::MDC_TRACKS,
                                                  HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
  task_manager.AddBranchCut(HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::META_HITS,
                                                  HadesUtils::DATA_TYPE::AuAu_1_23AGeV));

  task_manager.AddTask(task);
  task_manager.Init();
  task_manager.Run(-1);
  task_manager.Finish();
  return 0;
}
