

#ifndef INNERMOSTLOOPS_H
#define INNERMOSTLOOPS_H

#include "llvm/Analysis/LoopInfo.h"
#include <vector>


namespace pathprofiling {

std::vector<llvm::Loop*>
getInnermostLoops(llvm::LoopInfo &li);

}


#endif
