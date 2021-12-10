#include "libtok.h"

int
main(void)
{
    char *str = "abc\\&&lol$$ll\\$kkk\n\nqqq \\l";
	char *delims[] = { "&&", "$", "\n\n" };
	int map[]      = {  1,    0,    0    };
    char** tokens;
    int dest;
	libtok_tokenize(str, delims, 3, map, "\\", LIBTOK_USE_ESCAPES, LIBTOK_DELIMS_ARE_TOKENS, &tokens, &dest);

    for (int i = 0; i < dest; i++)
        printf("%s\n", tokens[i]);
}
