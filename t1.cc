#include "gglib.h"
using namespace gg;

int main() {

	dgraph s;
	for (int i=0; i<2*1000*1000; ++i) {
		for (int j=0; j<15; ++j) {
			s.add_link(i,j);
		}
	}
	// s.dump();
	// s.partitions[0].max = 999;
	// cout << s.find_partition(998) << endl;

	char cmd[256];
	sprintf(cmd, "cat /proc/%d/status", getpid());
	system(cmd);
}
