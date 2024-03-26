/*
 * PriorityQueueSearch.cpp
 *
 *  Created on: 10.09.2017
 *      Author: Daniel Höller
 */

#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdlib.h>

#include "../Model.h"
#include "../ProgressionNetwork.h"
#include "../intDataStructures/FlexIntStack.h"
#include "PriorityQueueSearch.h"

#ifdef PREFMOD
// preferred modifications
#include "AlternatingFringe.h"
#endif

#include <algorithm>
#include <bitset>
#include <map>
#include <queue>

namespace progression {

PriorityQueueSearch::~PriorityQueueSearch() {
  // TODO Auto-generated destructor stub
}

searchNode *PriorityQueueSearch::handleNewSolution(searchNode *newSol,
                                                   searchNode *oldSol,
                                                   long time) {
  searchNode *res;
  foundSols++;
  if (oldSol == nullptr) {
    res = newSol;
    firstSolTime = time;
    bestSolTime = time;
    if (this->optimzeSol) {
      cout << "SOLUTION: (" << time
           << "ms) Found first solution with action costs of "
           << newSol->actionCosts << "." << endl;
    }
  } else if (newSol->actionCosts < oldSol->actionCosts) {
    // shall optimize until time limit, this is a better one
    bestSolTime = time;
    res = newSol;
    solImproved++;
    cout << "SOLUTION: (" << time
         << "ms) Found new solution with action costs of "
         << newSol->actionCosts << "." << endl;
  } else {
    // cout << "Found new solution with action costs of " << newSol->actionCosts
    // << "." << endl;
    res = oldSol;
  }
  return res;
}

/// closing namespace
} // namespace progression
