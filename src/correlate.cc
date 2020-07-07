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

  st.AddQ1Q1Correlation("tracks_mdc", "wall_sub1");
  st.AddQ1Q1Correlation("tracks_mdc", "wall_sub2");

  st.AddQ1Q1Correlation("wall_sub1", "wall_sub2");

  auto start = std::chrono::system_clock::now();
  st.Run();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
