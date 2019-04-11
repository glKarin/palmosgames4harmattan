#include <stdio.h>
#include <math.h>

#define PD(x) printf(#x" -> %d\n", x)
#define PP(x) printf(#x" -> %p\n", x)

#define MEM_K 1008108

int main(void)
{
	PD(MEM_K * 1024);
	PP(MEM_K * 1024);
	return 0;
}
