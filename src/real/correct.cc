#include <iostream>
#include <boost/program_options.hpp>

#include <QnTools/Stats.hpp>
#include <GlobalConfig.h>
#include <CorrectTaskManager.h>

#include <AnalysisTree/Variable.hpp>

#include <cuts.h>

int main(int argc, char **argv) {
  using namespace std;
  namespace po = boost::program_options;
  if(argc < 2){
    throw std::runtime_error("No arguments were provided. Use ./correct --help to get more information.");
  }

  std::string file_list;
  po::options_description options("Options");
  options.add_options()
      ("help,h", "Help screen")
      ("debug,d", "Debug option: 1K events, no corrections of Q-vectors")
      ("input,i", po::value<std::string>(&file_list),
       "Set path to input configuration");
  po::variables_map vm;
  po::parsed_options parsed = po::command_line_parser(argc, argv).options(options).run();
  po::store(parsed, vm);
  po::notify(vm);
  if (vm.count("help")){
    std::cout << options << std::endl;
    return 0;
  }
  bool is_debug=vm.count("debug");

  const string event_header = "event_header";
  const string vtx_tracks = "mdc_vtx_tracks";
  const string wall_hits = "forward_wall_hits";

  double beam_rapidity = AnalysisTree::GetObjectFromFileList<AnalysisTree::DataHeader>(file_list, "DataHeader")->GetBeamRapidity();
  std::string system = AnalysisTree::GetObjectFromFileList<AnalysisTree::DataHeader>(file_list, "DataHeader")->GetSystem();

  AnalysisTree::Variable y_cm("y_cm",
                              {{vtx_tracks, "rapidity"}},
                              [beam_rapidity](const std::vector<double> &var){
                                return var.at(0)-beam_rapidity;
                              });

  Qn::AxisConfig pt_axis({vtx_tracks, "pT"}, 20, 0.0, 2.0);
  Qn::AxisConfig rapidity_axis(y_cm, 15, -0.75, 0.75);
  auto* global_config = new Qn::GlobalConfig();
  global_config->AddEventVar({event_header, "selected_tof_rpc_hits_centrality"});
  global_config->AddCorrectionAxis( {"event_header_selected_tof_rpc_hits_centrality", 8, 0.0, 40.0} );
  // un-vector from MDC
  Qn::QvectorTracksConfig un_vector("u",{vtx_tracks, "phi"}, {"Ones"},
                                    {pt_axis,rapidity_axis});
  un_vector.SetCorrectionSteps(true, true, true);
  if( is_debug )
    un_vector.SetCorrectionSteps(false, false, false);

  un_vector.AddCut({AnalysisTree::Variable("mdc_vtx_tracks", "geant_pid"),
                    [](double pid) { return abs(pid - 14.0) < 0.1; }, "cut on proton pid"});
  un_vector.SetType(Qn::Stats::Weights::OBSERVABLE);
  global_config->AddTrackQvector(un_vector);

  // Q-vectors from Forward Wall
  Qn::QvectorTracksConfig qn_wall_1("W1", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_wall_1.SetCorrectionSteps(true, true, true);
  if( is_debug )
    qn_wall_1.SetCorrectionSteps(false, false, false);
  qn_wall_1.AddCut({{wall_hits, "ring"},
                    [](double value) { return 1.0 <= value && value <= 5.0; }, "cut on first SE"});
  qn_wall_1.AddCut({{wall_hits, "beta"},
                    [](double value) { return 0.84 <= value && value <= 1.0; }, "cut on beta first SE"});
  qn_wall_1.AddCut({{wall_hits, "signal"},
                    [](double value) { return 80.0 <= value && value <= 999.0; }, "cut on signal first SE"});
  qn_wall_1.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_wall_1);

  Qn::QvectorTracksConfig qn_wall_2("W2", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_wall_2.SetCorrectionSteps(true, true, true);
  if( is_debug )
    qn_wall_2.SetCorrectionSteps(false, true, true);
  qn_wall_2.AddCut({{wall_hits, "ring"},
                    [](double value) { return 6.0 <= value && value <= 7.0; }, "cut on second SE"});
  qn_wall_2.AddCut({{wall_hits, "beta"},
                    [](double value) { return 0.85 <= value && value <= 1.0; }, "cut on beta second SE"});
  qn_wall_2.AddCut({{wall_hits, "signal"},
                    [](double value) { return 85.0 <= value && value <= 999.0; }, "cut on signal second SE"});
  qn_wall_2.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_wall_2);

  Qn::QvectorTracksConfig qn_wall_3("W3", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_wall_3.SetCorrectionSteps(true, true, true);
  if( is_debug )
    qn_wall_3.SetCorrectionSteps(false, false, false);
  qn_wall_3.AddCut({{wall_hits, "ring"},
                      [](double value){ return 8.0 <= value && value <= 10.0;}, "cut on third SE"});
  qn_wall_3.AddCut({{wall_hits, "beta"},
                    [](double value) { return 0.80 <= value && value <= 1.0; }, "cut on beta third SE"});
  qn_wall_3.AddCut({{wall_hits, "signal"},
                    [](double value) { return 88.0 <= value && value <= 999.0; }, "cut on signal third SE"});
  qn_wall_3.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_wall_3);

  Qn::QvectorTracksConfig qn_rs1("R1", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_rs1.SetCorrectionSteps(true, false, false);
  if( is_debug )
    qn_rs1.SetCorrectionSteps(false, false, false);
  qn_rs1.AddCut({{wall_hits, "rnd_sub"},
                      [](double value){ return fabs(value - 0.0) < 0.1;},"cut on first RND-SE"});
  qn_rs1.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_rs1);
  Qn::QvectorTracksConfig qn_rs2("R2", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_rs2.SetCorrectionSteps(true, false, false);
  if( is_debug )
    qn_rs2.SetCorrectionSteps(false, false, false);
  qn_rs2.AddCut({{wall_hits, "rnd_sub"},
                      [](double value){ return fabs(value - 1.0) < 0.1;}, "cut on second RND-SE"});
  global_config->AddTrackQvector(qn_rs2);

  Qn::QvectorTracksConfig qn_full("F", {wall_hits, "phi"},
                                       {wall_hits, "signal"},{});
  qn_full.SetCorrectionSteps(true, false, false);
  if( is_debug )
    qn_full.SetCorrectionSteps(false, false, false);
  qn_full.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_full);
  // Q-vector from MDC
  Qn::QvectorTracksConfig qn_mdc_f("Mf",
                                    {vtx_tracks, "phi"},
                                   {"Ones"}, {});
  qn_mdc_f.SetCorrectionSteps(true, true, true);
  if( is_debug )
    qn_mdc_f.SetCorrectionSteps(false, false, false);
  qn_mdc_f.AddCut( {AnalysisTree::Variable("mdc_vtx_tracks", "geant_pid"),
                    [](double pid) { return abs(pid - 14.0) < 0.1; }, "proton cut"} );
  qn_mdc_f.AddCut( {AnalysisTree::Variable("mdc_vtx_tracks", "rapidity"),
                    [](double rapidity) { return 1.09 < rapidity && rapidity < 1.29; }, "forward cut"} );
  qn_mdc_f.AddCut( {AnalysisTree::Variable("mdc_vtx_tracks", "pT"),
                    [](double pT) { return 0.0 < pT && pT < 2.0; }, "forward cut"} );
  qn_mdc_f.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_mdc_f);

  Qn::QvectorTracksConfig qn_mdc_b("Mb",
                                    {vtx_tracks, "phi"},
                                   {"Ones"}, {});
  qn_mdc_b.SetCorrectionSteps(true, true, true);
  if( is_debug )
    qn_mdc_b.SetCorrectionSteps(false, false, false);
  qn_mdc_b.AddCut( {AnalysisTree::Variable("mdc_vtx_tracks", "geant_pid"),
                    [](double pid) { return abs(pid - 14.0) < 0.1; }, "proton cut"} );
  qn_mdc_b.AddCut( {AnalysisTree::Variable("mdc_vtx_tracks", "rapidity"),
                    [](double rapidity) { return 0.19 < rapidity && rapidity < 0.39; }, "backward cut"} );
  qn_mdc_b.AddCut( {AnalysisTree::Variable("mdc_vtx_tracks", "pT"),
                    [](double pT) { return 0.0 < pT && pT < 2.0; }, "forward cut"} );
  qn_mdc_b.SetType(Qn::Stats::Weights::REFERENCE);
  global_config->AddTrackQvector(qn_mdc_b);

 // ***********************************************
  // first filelist should contain DataHeader

  Qn::CorrectTaskManager task_manager({file_list}, {"hades_analysis_tree"});
  auto* task = new Qn::CorrectionTask(global_config);

  AnalysisTree::Variable eta(vtx_tracks, "eta");

  task->AddQAHistogram("u", {{"y_cm", 200, -0.75+beam_rapidity, 0.75+beam_rapidity},
                           {vtx_tracks + "_pT", 200, 0.0, 2.0}});

  task->AddQAHistogram("u", {{vtx_tracks + "_pT", 200, 0.0, 2.0},
                             {vtx_tracks + "_phi", 315, -3.15, 3.15}});

  task->AddQAHistogram("u", {{"y_cm", 100, -0.75+beam_rapidity, 0.75+beam_rapidity},
                           {vtx_tracks + "_phi", 315, -3.15, 3.15}});

  if( system == "Au+Au" ) {
    task_manager.SetEventCuts(
        HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::EVENT_HEADER,
                              HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
    task_manager.AddBranchCut(
        HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::MDC_TRACKS,
                              HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
    task_manager.AddBranchCut(
        HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::META_HITS,
                              HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
//    task_manager.AddBranchCut(
//        HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::WALL_HITS,
//                              HadesUtils::DATA_TYPE::AuAu_1_23AGeV));
  }else if( system=="Ag+Ag" ){
    task_manager.SetEventCuts(
        HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::EVENT_HEADER,
                              HadesUtils::DATA_TYPE::AgAg_1_23AGeV));
    task_manager.AddBranchCut(
        HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::MDC_TRACKS,
                              HadesUtils::DATA_TYPE::AgAg_1_23AGeV));
    task_manager.AddBranchCut(
        HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::META_HITS,
                              HadesUtils::DATA_TYPE::AgAg_1_23AGeV));
//    task_manager.AddBranchCut(
//        HadesUtils::Cuts::Get(HadesUtils::Cuts::BRANCH_TYPE::WALL_HITS,
//                              HadesUtils::DATA_TYPE::AgAg_1_23AGeV));
  }
  task_manager.AddTask(task);
  task_manager.Init();
  if( is_debug )
    task_manager.Run(10000);
  else
    task_manager.Run(-1);
  task_manager.Finish();
  return 0;
}
