//
// Created by dh on 29.03.21.
//

#ifndef PANDAPIENGINE_ONEQUEUEWASTARFRINGE_H
#define PANDAPIENGINE_ONEQUEUEWASTARFRINGE_H

class OneQueueWAStarFringe;

#include "../../ProgressionNetwork.h"
#include <queue>

enum aStar { gValNone, gValPathCosts, gValActionCosts, gValActionPathCosts };

struct TieBreakingNodePointerComaprator {
  int numOfHeuristics;
  TieBreakingNodePointerComaprator(int hNum) : numOfHeuristics(hNum) {}
  /* compatator for search node pointers*/
  bool operator()(const searchNode *a, const searchNode *b) const;
};

class OneQueueWAStarFringe {
public:
  OneQueueWAStarFringe(aStar _aStarOption, int _hWeight, int numberOfHeuristics)
      : aStarOption(_aStarOption), hWeight(_hWeight),
        fringe(TieBreakingNodePointerComaprator(numberOfHeuristics)){};
  bool isEmpty();
  searchNode *pop();
  void push(searchNode *n);
  int size();

  void printTypeInfo();
  void push1(searchNode *n);
  void make_heap();
private:
  aStar aStarOption;
  int hWeight;

  priority_queue<searchNode *, vector<searchNode *>,
                 TieBreakingNodePointerComaprator>
      fringe;
  std::vector<searchNode*> fringe1;
};

#endif // PANDAPIENGINE_ONEQUEUEWASTARFRINGE_H
