#include <iostream>

#include <GlobalConfig.h>

#include <CorrectTaskManager.h>
#include <QnTools/Stats.hpp>

#include <AnalysisTree/Variable.hpp>

#include <centrality.h>
#include <cuts.h>
#include <corrections.h>

int main(int argc, char **argv) {
  using namespace std;
  if(argc < 2){
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./correct filelist.txt path/to/efficiency.root" << std::endl;
    exit(EXIT_FAILURE);
  }
  const string file_list = argv[1];
  const string eff_file = argv[2];

  const string event_header = "event_header";
  const string vtx_tracks = "mdc_vtx_tracks";
  const string sim_tracks = "sim_tracks";
  const string sim_event = "sim_header";

  AnalysisTree::Variable centrality("Centrality",
                                    {{event_header, "selected_tof_rpc_hits"}},
                                    [](const std::vector<double> &var){
                                      return HadesUtils::Centrality::GetValue(var.at(0),
                                                                              HadesUtils::DATA_TYPE::AuAu_1_23AGeV);});
  HadesUtils::Corrections::ReadMaps(eff_file);
  AnalysisTree::Variable efficiency("efficiency",
                                    {{vtx_tracks, "rapidity"},
                                     {vtx_tracks, "pT"}},
                                    [](const std::vector<double> &var){
//                                      auto cent_class = HadesUtils::Centrality::GetClass(var.at(0),
//                                                                              HadesUtils::DATA_TYPE::AuAu_1_23AGeV);
//                                      if( cent_class > 7 )
//                                        return 1.0;
                                      auto y = var.at(0);
                                      auto pT = var.at(1);
                                      auto eff = HadesUtils::Corrections::GetEfficiency(0, pT, y);
                                      if( eff < 1e-2 )
                                        return 0.0;
                                      return 1.0/eff;
                                    });
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

  Qn::QvectorTracksConfig pid_reco_eff("PID_Eff_Corr",
                                  {vtx_tracks, "phi"}, efficiency,
                                  {pt_axis, rapidity_axis});
  pid_reco_eff.SetCorrectionSteps(false, false, false);
  pid_reco_eff.AddCut( {AnalysisTree::Variable(vtx_tracks, "geant_pid"),
                       [](double pid) { return abs(pid - 14.0) < 0.1; }, "PID_Eff_Corr, cut on proton reco-pid"});
  pid_reco_eff.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(pid_reco_eff);

  Qn::QvectorTracksConfig pid_reco_no_eff("PID_No_Eff_Corr",
                                  {vtx_tracks, "phi"}, {"Ones"},
                                  {pt_axis, rapidity_axis});
  pid_reco_no_eff.SetCorrectionSteps(false, false, false);
  pid_reco_no_eff.AddCut( {AnalysisTree::Variable(vtx_tracks, "geant_pid"),
                       [](double pid) { return abs(pid - 14.0) < 0.1; }, "PID_No_Eff_Corr, cut on proton reco-pid"});
  pid_reco_no_eff.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(pid_reco_no_eff);

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
  task_manager.Run(1000);
  task_manager.Finish();
  return 0;
}
