#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <stropts.h>

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("usage: %s <pathname>\n", argv[0]);
		exit(1);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		printf("can't open %s\n", argv[1]);
		exit(1);
	}
	if (isastream(fd) == 0) {
		printf("%s is not a stream\n", argv[1]);
		exit(1);
	}

	/* get nmods */
	int nmods = ioctl(fd, I_LIST, 0);
	if (nmods < 0) {
		printf("I_LIST error for nmods\n");
		exit(1);
	}
	printf("#modules = %d\n", nmods);

	struct str_list list;
	list.sl_modlist = calloc(nmods, sizeof(struct str_mlist));
	if (!list.sl_modlist) {
		printf("calloc error\n");
		exit(1);
	}
	list.sl_nmods = nmods;

	if (ioctl(fd, I_LIST, &list) < 0) {
		printf("I_LIST error for list\n");
		exit(1);
	}

	int i;
	for(i = 1; i <= nmods; i++) printf("%s %s\n", (i==nmods) ? "driver" : "module", list.sl_modlist++->l_name);

	return 0;
}
