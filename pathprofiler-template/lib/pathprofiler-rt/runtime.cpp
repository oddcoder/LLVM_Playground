
#include <cstdint>
#include <cstdio>


extern "C" {


// This macro allows us to prefix strings so that they are less likely to
// conflict with existing symbol names in the examined programs.
// e.g. EPP(entry) yields PaThPrOfIlInG_entry
#define EPP(X)  PaThPrOfIlInG_ ## X


extern uint64_t EPP(numPaths);

extern uint64_t EPP(pathCounts)[];


void
EPP(logPath)(uint64_t loopID, uint64_t pathID) {
  // TODO: implement
}


void
EPP(save)() {
  // TODO: implement
}


}

