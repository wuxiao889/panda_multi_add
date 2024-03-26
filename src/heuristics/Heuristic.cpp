//
// Created by dh on 29.03.21.
//

#include "Heuristic.h"
#include <algorithm>
#include <numeric>

std::vector<long long> Heuristic::exec_times;

void Heuristic::setHeruAndCalcTime(searchNode *n, searchNode *parent,
                                   int absTask, int method) {
  auto start = high_resolution_clock::now();
  setHeuristicValue(n, parent, absTask, method);
  auto end = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(end - start).count();
  exec_times.push_back(duration);
}

void Heuristic::setHeruAndCalcTime(searchNode *n, searchNode *parent,
                                   int action) {
  auto start = high_resolution_clock::now();
  setHeuristicValue(n, parent, action);
  auto end = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(end - start).count();
  exec_times.push_back(duration);
}

long long Heuristic::getSum() {
  return accumulate(exec_times.begin(), exec_times.end(), 0LL);
}

void Heuristic::printInfo() {
  auto maxTime = *max_element(exec_times.begin(), exec_times.end());
  auto minTime = *min_element(exec_times.begin(), exec_times.end());
  auto totalTime = accumulate(exec_times.begin(), exec_times.end(), 0LL);
  auto avgTime = totalTime / exec_times.size();

  cout << "Execution times: " << exec_times.size() << endl;
  cout << "Max execution time: " << maxTime << " microseconds" << endl;
  cout << "Min execution time: " << minTime << " microseconds" << endl;
  cout << "Total execution time: " << totalTime << " microseconds" << endl;
  cout << "Average execution time: " << avgTime << " microseconds" << endl;
}
