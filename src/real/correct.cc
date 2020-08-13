#include <iostream>
#include <chrono>

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
  const string wall_hits = "forward_wall_hits";

  // Configuration will be here

  AnalysisTree::Variable centrality("Centrality",
                                    {{event_header, "selected_tof_rpc_hits"}},
                                    [](const std::vector<double> &var){
                                      return HadesUtils::Centrality::GetValue(var.at(0),
                                                                              HadesUtils::DATA_TYPE::AuAu_1_23AGeV);});

  double beam_rapidity = AnalysisTree::GetObjectFromFileList<AnalysisTree::DataHeader>(file_list, "data_information")->GetBeamRapidity();
  std::string system = AnalysisTree::GetObjectFromFileList<AnalysisTree::DataHeader>(file_list, "data_information")->GetSystem();

  AnalysisTree::Variable y_cm("y_cm",
                              {{vtx_tracks, "rapidity"}},
                              [beam_rapidity](const std::vector<double> &var){
                                return var.at(0)-beam_rapidity;
                              });

  Qn::AxisConfig pt_axis({vtx_tracks, "pT"}, 16, 0.0, 1.6);
  Qn::AxisConfig rapidity_axis(y_cm, 15, -0.75, 0.75);
  auto* global_config = new Qn::GlobalConfig();
  global_config->AddEventVar(centrality);
  global_config->AddCorrectionAxis( {"Centrality", 8, 0.0, 40.0} );
  // un-vector from MDC
  Qn::QvectorTracksConfig un_vector("u",{vtx_tracks, "phi"}, {"Ones"},
                                    {pt_axis,rapidity_axis});
  un_vector.SetCorrectionSteps(true, true, true);

  un_vector.AddCut({AnalysisTree::Variable("mdc_vtx_tracks", "geant_pid"),
                    [](double pid) { return abs(pid - 14.0) < 0.1; }, "cut on proton pid"});
  un_vector.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(un_vector);

  // Q-vectors from Forward Wall
  Qn::QvectorTracksConfig qn_wall_1("W1", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_wall_1.SetCorrectionSteps(true, false, false);
  qn_wall_1.AddCut({{wall_hits, "ring"},
                    [](double value) { return 1.0 <= value && value <= 5.0; }, "cut on first SE"});
  qn_wall_1.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_wall_1);

  Qn::QvectorTracksConfig qn_wall_2("W2", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_wall_2.SetCorrectionSteps(true, false, false);
  qn_wall_2.AddCut({{wall_hits, "ring"},
                    [](double value) { return 6.0 <= value && value <= 7.0; }, "cut on second SE"});
  qn_wall_2.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_wall_2);

  Qn::QvectorTracksConfig qn_wall_3("W3", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_wall_3.SetCorrectionSteps(true, false, false);
  qn_wall_3.AddCut({{wall_hits, "ring"},
                      [](double value){ return 8.0 <= value && value <= 10.0;}, "cut on third SE"});
  qn_wall_3.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_wall_3);

  Qn::QvectorTracksConfig qn_rs1("R1", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_rs1.SetCorrectionSteps(true, false, false);
  qn_rs1.AddCut({{wall_hits, "rnd_sub"},
                      [](double value){ return fabs(value - 0.0) < 0.1;},"cut on first RND-SE"});
  qn_rs1.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_rs1);
  Qn::QvectorTracksConfig qn_rs2("R2", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_rs2.SetCorrectionSteps(true, false, false);
  qn_rs2.AddCut({{wall_hits, "rnd_sub"},
                      [](double value){ return fabs(value - 1.0) < 0.1;}, "cut on second RND-SE"});
  global_config->AddTrackQvector(qn_rs2);

  Qn::QvectorTracksConfig qn_full("F", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_full.SetCorrectionSteps(true, false, false);
  qn_full.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_full);
  // Q-vector from MDC
  Qn::QvectorTracksConfig qn_mdc("M",
                                    {vtx_tracks, "phi"}, {"Ones"},
                                    {rapidity_axis});
  qn_mdc.SetCorrectionSteps(true, true, true);
  qn_mdc.AddCut( {AnalysisTree::Variable("mdc_vtx_tracks","geant_pid"),
                    [](double pid) { return abs(pid - 14.0) < 0.1; }, "proton cut"} );
  qn_mdc.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_mdc);

 // ***********************************************
  // first filelist should contain DataHeader

  Qn::CorrectTaskManager task_manager({file_list}, {"hades_analysis_tree"});
  auto* task = new Qn::CorrectionTask(global_config);

  AnalysisTree::Variable eta(vtx_tracks, "eta");

  task->AddQAHistogram("u", {{vtx_tracks + "_rapidity", 200, -0.75+beam_rapidity, 0.75+beam_rapidity},
                           {vtx_tracks + "_pT", 200, 0.0, 2.0}});

  task->AddQAHistogram("u", {{vtx_tracks + "_pT", 200, 0.0, 2.0},
                             {vtx_tracks + "_phi", 315, -3.15, 3.15}});

  task->AddQAHistogram("u", {{vtx_tracks + "_rapidity", 100, -0.75+beam_rapidity, 0.75+beam_rapidity},
                           {vtx_tracks + "_phi", 315, -3.15, 3.15}});

  // TODO fix hardcode, change according to data header's information
  task_manager.SetEventCuts(HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::EVENT_HEADER,
                                                  HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
  task_manager.AddBranchCut(HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::MDC_TRACKS,
                                                  HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
  task_manager.AddBranchCut(HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::META_HITS,
                                                  HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
  task_manager.AddBranchCut(HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::WALL_HITS,
                                                  HadesUtils::DATA_TYPE::AuAu_1_23AGeV));

  task_manager.AddTask(task);
  task_manager.Init();
  task_manager.Run(-1);
  task_manager.Finish();
  return 0;
}
