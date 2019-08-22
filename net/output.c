#include "inc/lib.h"
#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	envid_t envid = 0;
        int perm = 0;

        while (1) {
		if (ipc_recv((envid_t *)&envid, &nsipcbuf, &perm) != NSREQ_OUTPUT) {
			continue;
		}


		while (sys_package_send((void *)nsipcbuf.pkt.jp_data, (size_t)nsipcbuf.pkt.jp_len) < 0) {
			sys_yield();
		}
	}
}
