#include <iostream>
#include <chrono>
#include <string>

#include <CorrelationTask.h>

int main(int argc, char **argv) {
  using namespace std;

  if(argc < 2){
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./correlate file.root" << std::endl;
    exit(EXIT_FAILURE);
  }

  const std::string& file{argv[1]};
  CorrelationTask st(file, "tree");
  st.SetNonZeroOnly(true);
//  st.AddQ2Q2Correlation("PID_Eff_Corr", "psi_rp");
//  st.AddQ2Q2Correlation("PID_No_Eff_Corr", "psi_rp");

  auto start = std::chrono::system_clock::now();
  st.Run();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
