/*
 * sAddFF.cpp
 *
 *  Created on: 23.09.2017
 *      Author: Daniel Höller
 */

#include "hsAddFF.h"
#include <cassert>
#include <cstring>

namespace progression {

hsAddFF::hsAddFF(Model *sas) {
  heuristic = sasFF;
  assert(!sas->isHtnModel);
  m = sas;
  hValPropInit = new hType[m->numStateBits];
  for (int i = 0; i < m->numStateBits; i++) {
    hValPropInit[i] = hUnreachable;
  }
  queue = new IntPairHeap<hType>(m->numStateBits * 2);
  numSatPrecs = new int[m->numActions];
  hValOp = new hType[m->numActions];
  hValProp = new hType[m->numStateBits];
  reachedBy = new int[m->numStateBits];
  markedFs.init(m->numStateBits);
  markedOps.init(m->numActions);
  needToMark.init(m->numStateBits);
  allActionsCostOne = true;
  for (int i = 0; i < m->numActions; i++) {
    if (m->actionCosts[i] != 1) {
      allActionsCostOne = false;
      break;
    }
  }
  assert(assertPrecAddDelSets());
}

bool hsAddFF::assertPrecAddDelSets() {
  for (int i = 0; i < m->numActions; i++) {
    set<int> testset;
    for (int j = 0; j < m->numPrecs[i]; j++) {
      testset.insert(m->precLists[i][j]);
    }
    assert(m->numPrecs[i] == testset.size());
    testset.clear();

    for (int j = 0; j < m->numAdds[i]; j++) {
      testset.insert(m->addLists[i][j]);
    }
    assert(m->numAdds[i] == testset.size());
    testset.clear();

    for (int j = 0; j < m->numDels[i]; j++) {
      testset.insert(m->delLists[i][j]);
    }
    assert(m->numDels[i] == testset.size());
  }
  return true;
}

hsAddFF::~hsAddFF() {
  delete[] hValPropInit;
  delete[] numSatPrecs;
  delete[] hValOp;
  delete[] hValProp;
  delete[] reachedBy;
  delete queue;
}

hType hsAddFF::getFF(noDelIntSet &g) {
  // FF extraction
  markedFs.clear();
  markedOps.clear();
  needToMark.clear();
  for (int f = g.getFirst(); f >= 0; f = g.getNext()) {
    assert(hValProp[f] != hUnreachable);
    needToMark.push(f);
    while (!needToMark.isEmpty()) {
      int someF = needToMark.pop();
      if (markedFs.get(someF)) {
        continue;
      }
      markedFs.insert(someF);
      if (reachedBy[someF] != NOACTION) {
        // else it is set in s0
        for (int i = 0; i < m->numPrecs[reachedBy[someF]]; i++) {
          int prec = m->precLists[reachedBy[someF]][i];
          needToMark.push(prec);
        }
        markedOps.insert(reachedBy[someF]);
      }
    }
  }
  if (allActionsCostOne) {
    return markedOps.getSize();
  } else {
    hType hVal = 0;
    for (int op = markedOps.getFirst(); op >= 0; op = markedOps.getNext()) {
      hVal += m->actionCosts[op];
    }
    return hVal;
  }
}

int hsAddFF::getHeuristicValue(bucketSet &s, noDelIntSet &g) {
  if (g.getSize() == 0)
    return 0;
  hType hVal = hUnreachable;

  memcpy(numSatPrecs, m->numPrecs, sizeof(int) * m->numActions);
  for (int i = 0; i < m->numActions; i++) {
    hValOp[i] = m->actionCosts[i];
  }
  memcpy(hValProp, hValPropInit, sizeof(hType) * m->numStateBits);

  int numGoals = g.getSize();

  queue->clear();
  for (int f = s.removeFirst(); f >= 0; f = s.removeNext()) {
    queue->add(0, f);
    hValProp[f] = 0;
    reachedBy[f] = NOACTION;
  }

  // 对所有无前提条件的动作
  for (int i = 0; i < m->numPrecLessActions; i++) {
    int ac = m->precLessActions[i];
    // 对所有动作ac的加性条件fAdd
    for (int iAdd = 0; iAdd < m->numAdds[ac]; iAdd++) {
      int fAdd = m->addLists[ac][iAdd];
      // 这里或许应该和原来比取最小值
      hValProp[fAdd] = m->actionCosts[ac];
      reachedBy[fAdd] = ac;
      queue->add(hValProp[fAdd], fAdd);
    }
  }
  while (!queue->isEmpty()) {
    hType pVal = queue->topKey();
    assert(pVal >= 0);
    int prop = queue->topVal();
    queue->pop();
    if (hValProp[prop] < pVal)
      continue;
    // 如果目标状态g包含了prop
    if (g.get(prop)) {
      // 如果所有目标状态都满足了
      if (--numGoals == 0) {
        // hadd
        if (heuristic == sasAdd) {
          hVal = 0;
          // 对目标状态g的所有状态启发值求和
          for (int f = g.getFirst(); f >= 0; f = g.getNext()) {
            assert(hValProp[f] != hUnreachable);
            hVal += hValProp[f];
          }
          break;
        } else { // FF extraction
          hVal = getFF(g);
          break;
        }
      }
    }
    // 不用容器，很操蛋
    for (int iOp = 0; iOp < m->precToActionSize[prop]; iOp++) {
      // 动作op前提条件包含了s
      int op = m->precToAction[prop][iOp];
      hType newVal;
      if (this->heuristic == sasFF) {
        newVal = max(hValOp[op], m->actionCosts[op] + pVal);
      } else {
        // hadd 动作op的启发式值 为 所有前置条件的启发式值值和
        newVal = hValOp[op] + pVal;
      }

      if ((newVal < hValOp[op]) || (newVal < pVal)) {
        if (!this->reportedOverflow) {
          cout << "WARNING: Integer overflow in hAdd/hFF calculation. Value "
                  "has been cut."
               << endl;
          cout << "         You can choose a different data type for the "
                  "Add/FF calculation (look for \"hType\" in heuristic class)"
               << endl;
          cout << "         This message will only be reported once!" << endl;
          this->reportedOverflow = true;
        }
        return INT_MAX - 2; // here, the external data type for heuristic values
                            // (i.e. int) must be used
      }
      hValOp[op] = newVal;

      assert(hValOp[op] >= 0);
      // 动作op的所有前提条件都满足了
      if (--numSatPrecs[op] == 0) {
        // 为什么不用容器
        for (int iF = 0; iF < m->numAdds[op]; iF++) {
          // 动作op的第iF个add状态
          int f = m->addLists[op][iF];
          // 如果op(已经加上 op cost)比原来状态f的启发式状态值小
          if (hValOp[op] < hValProp[f]) {
            hValProp[f] = hValOp[op];
            reachedBy[f] = op; // only used by FF
            queue->add(hValProp[f], f);
          }
        }
      }
    }
  }
  if (hVal == hUnreachable) {
    return UNREACHABLE;
  } else if (hVal >= INT_MAX) {
    if (!this->reportedOverflow) {
      cout << "WARNING: Integer overflow in hAdd/hFF calculation. Value has "
              "been cut."
           << endl;
      cout << "         You can choose a different data type for the Add/FF "
              "calculation (look for \"hType\" in heuristic class)"
           << endl;
      cout << "         This message will only be reported once!" << endl;
      this->reportedOverflow = true;
    }
    return INT_MAX - 2;
  } else {
    return (int)hVal;
  }
}

} // namespace progression
/* namespace progression */
