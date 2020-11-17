#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int main(int argc, char **argv)
{
    if (argc != EXPECTED_ARGC) {
	fprintf("Usage: %s <file-to-infect>", argv[0]);
	exit(1);
    }
}
