#include "vendor.h"
#include "runner.h"
#include "coroutine.h"

int main(int argc, char* argv[]) {
    G = new runner();
    G->init(argc, argv);
    G->run();
    return 0;
}
