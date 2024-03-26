//
// Created by dh on 29.03.21.
//

#ifndef PANDAPIENGINE_HEURISTIC_H
#define PANDAPIENGINE_HEURISTIC_H

#include "../Model.h"
#include <chrono>
using namespace std::chrono;

class Heuristic {
protected:
  const Model *const htn;
  const int index;
  static std::vector<long long> exec_times;

public:
  Heuristic(Model *htnModel, int index): htn(htnModel), index(index) {}

  // returns textual description of the heuristic for output
  virtual string getDescription() = 0;

  void setHeruAndCalcTime(searchNode *n, searchNode *parent,
                                 int action);
  void setHeruAndCalcTime(searchNode *n, searchNode *parent, int absTask,
                                 int method);
  static void printInfo();
  static long long getSum();

  virtual void setHeuristicValue(searchNode *n, searchNode *parent,
                                 int action) = 0;
  virtual void setHeuristicValue(searchNode *n, searchNode *parent, int absTask,
                                 int method) = 0;
};

#endif // PANDAPIENGINE_HEURISTIC_H
