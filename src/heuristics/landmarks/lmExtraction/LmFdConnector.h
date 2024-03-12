/*
 * LmFdConnector.h
 *
 *  Created on: 09.02.2020
 *      Author: dh
 */

#ifndef HEURISTICS_LANDMARKS_LMFDCONNECTOR_H_
#define HEURISTICS_LANDMARKS_LMFDCONNECTOR_H_

#include "../../../Model.h"
#include "../../../intDataStructures/StringUtil.h"
#include "../../rcHeuristics/RCModelFactory.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>

namespace progression {

class LmFdConnector {
public:
  LmFdConnector();
  virtual ~LmFdConnector();

  void createLMs(Model *htn);

  int numLMs = -1;
  int numConjunctive = -1;
  landmark **landmarks = nullptr;

  int getNumLMs();
  landmark **getLMs();

private:
  StringUtil su;

  void readFDLMs(string f, RCModelFactory *factory);
  int getIndex(string f, Model *rc);
};

} /* namespace progression */

#endif /* HEURISTICS_LANDMARKS_LMFDCONNECTOR_H_ */
