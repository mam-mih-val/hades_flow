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
  st.SetNonZeroOnly(false);

  st.AddQ1Q1Correlation("u", "W1", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("u", "W2", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("u", "W3", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("u", "Mf", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("u", "Mb", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("u", "F", CorrelationTask::u1Q1_EVENT_PLANE);

  st.AddQ1Q1Correlation("W1", "W2", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("W1", "W3", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("W2", "W3", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("R1", "R2", CorrelationTask::Q1Q1_EVENT_PLANE);

  st.AddQ1Q1Correlation("Mf", "W1", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("Mf", "W2", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("Mf", "W3", CorrelationTask::SCALAR_PRODUCT);

  st.AddQ1Q1Correlation("Mb", "W1", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("Mb", "W2", CorrelationTask::SCALAR_PRODUCT);
  st.AddQ1Q1Correlation("Mb", "W3", CorrelationTask::SCALAR_PRODUCT);

  st.AddQ1Q1Correlation("Mf", "Mb", CorrelationTask::SCALAR_PRODUCT);

  auto start = std::chrono::system_clock::now();
  st.Run();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
