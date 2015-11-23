#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

int (*orig_strcmp)(const char *s1, const char *s2);
int strcmp(const char *s1, const char *s2)
{
	printf("%s vs %s\n", s1, s2);
	return orig_strcmp(s1, s2);
}




void _init(void)
{
	printf("Loading %s.\n", __FILE__);
	orig_strcmp = dlsym(RTLD_NEXT, "strcmp");
}
