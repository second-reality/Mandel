#include "gfx.h"
#include "args.h"

int main(int argc, char* argv[]) 
{
	args_read(argc, argv);
	gfx_start();
	return 0;
}
