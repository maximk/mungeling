//
// The utility extracts two values - secret1 and secret2 - Erlang on Xen nodes
// need to connect to diod servers. The values play their part in MUNGE
// authentication.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "sha1.h"

void help(void)
{
	printf("mungeling v0.1.0\n");
	printf("The utility extracts two values needed by LING node to connect to diod\n"
		   "servers. Its output can be directly added to the command line of the node.\n"
		   "Options:\n"
		   "    -h            this help\n"
		   "    -f key_file   use key_file (default is /etc/munge/munge.key)\n");
	exit(0);
}

void usage(void)
{
	printf("Usage: mungeling [-h] [-f key_file]\n");
	exit(1);
}

void extract_secrets(uint8_t sec1[SHA1HashSize],
					 uint8_t sec2[SHA1HashSize], char *key_file)
{
	uint8_t buf[8192];
	FILE *f;
	
	if ((f = fopen(key_file, "r")) == 0)
	{
		perror("Error: cannot access key file");
		exit(1);
	}

	int size = fread(buf, 1, sizeof(buf), f);
	assert(size >= 0 && size < sizeof(buf));
	fclose(f);

	SHA1Context ctx1, ctx2;
	SHA1Reset(&ctx1);
	SHA1Reset(&ctx2);

	SHA1Input(&ctx1, buf, size);
	SHA1Input(&ctx2, buf, size);

	SHA1Input(&ctx1, "2", 1);
	SHA1Input(&ctx2, "1", 1);

	SHA1Result(&ctx1, sec1);
	SHA1Result(&ctx2, sec2);
}

char digit(uint8_t v)
{
	if (v <= 9)
		return '0' +v;
	else
		return 'a' +v -10;
}

void hex_str(uint8_t *data, int size, char *hex)
{
	char *out = hex;
	uint8_t *ptr = data;
	while (ptr < data +size)
	{
		*out++ = digit((*ptr) >> 4);
		*out++ = digit((*ptr) & 15);
		ptr++;
	}
	*out++ = 0;
}

void main(int ac, char *av[])
{
	char *key_file = "/etc/munge/munge.key";

	int c;
	while ((c = getopt(ac, av, "hf:")) != -1)
	{
		switch (c)
		{
		case 'h':
			help();

		case 'f':
			key_file = optarg;
			break;

		default:
			usage();
		}
	}

	uint8_t sec1[SHA1HashSize];
	uint8_t sec2[SHA1HashSize]; 
	extract_secrets(sec1, sec2, key_file);

	char buf1[64];
	char buf2[64];
	hex_str(sec1, SHA1HashSize, buf1);
	hex_str(sec2, SHA1HashSize, buf2);

	printf("-secret %s %s\n", buf1, buf2);
}

//EOF
