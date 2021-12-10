/*
 * current bugs:
 * escape string twice should insert itself
 * csp_esc is iffy
 *
 * TODO:
 * check for memory leaks with valgrind
 */

#ifndef LIBTOK_H
#define LIBTOK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define LIBTOK_USE_ESCAPES 1
#define LIBTOK_NO_ESCAPES 0

#define LIBTOK_DELIMS_ARE_TOKENS 1
#define LIBTOK_DELIMS_NOT_TOKENS 0

/*
 * This function's arguments are a bit hard to explain.
 *
 * This function splits string str by any of the deliminator present in char **deliminators.
 * csp_esc means "corresponding escapes", and is a list of boolean values the same length as
 * char **delims.
 *
 * Given index i,
 * if csp_esc[i] is true, ignore the current token
 * if char *escape_str is before it.
 *
 * EX:
 * If "@" is a deliminator, 
 * and "\" is the escape string
 * the string "abc\@def" would *not* be split,
 *
 * but "abc@def" would return { "abc", "def" } (assuming delims_as_tokens is false - this is explained further down)
 *
 * To not use any escapes, pass NULL as csp_esc, escape_str, or 0 for have_escapes.
 *
 * delims_as_tokens means that (if true) use the deliminators as tokens in the final list,
 * othewise ignore them.
 *
 * return code of 0 is success, anything else means something went wrong.
 * 
 * dest will be set to a malloc'd list of the tokens - dest_ntokens will be set to the number of tokens.
 */
int
lt_tokenize(char *str, char **delims, int n, int *csp_esc, 
        char *escape_str, int have_escapes, int delims_as_tokens,
        char ***dest, int *dest_ntokens)
{
    int rc = 0;  /* return code */

    /*
     * The following arrays are defined as follows:
     *
     * int *starts represents an array of integers, each of which pertaining to an index of the start of a token.
     *
     * int *ends is the same as *starts, but instead of the start indices, it is the ending indices.
     *
     * For example:
     *  starts[0] = 3;
     *  ends[0] = 5;
     *
     * would mean that the first token starts at index 3 (in char *str) and ends at index 5.
     *
     * allocated is the number of integers corresponding to tokens allocated.
     *
     * I personally find that this approach is much easier to work with than a struct.
     */

    int *starts;
    int *ends;

    int allocated = 256;

    int current_starts = 0;
    int current_ends = 0;

    /* string tokens */
    char **tokens;
    int allocated_tokens = 256;
    int current_token = 0;
    tokens = malloc(sizeof(char*) * allocated_tokens);

    /* allocate the int arrays */
    starts = malloc(sizeof(int) * allocated);
    ends = malloc(sizeof(int) * allocated);
    if (starts == NULL || ends == NULL || tokens == NULL) {
        rc = 1;
        goto end;
    }

    /* other vars */
    int check_for_escapes = (have_escapes || escape_str == NULL || csp_esc == NULL);

    /* build lists of indices (int *starts, int *ends) */
    for (size_t i = 0; i < strlen(str); i++) {
        char *str_with_offset = str + i;
        int matching = 0;
        int delim_len = 0;
        int is_escaped = 0;
        if (check_for_escapes) {  /* check for a preceding escape character */
            int escape_offset = i - strlen(escape_str);

            char *str_before_token = str + escape_offset;

            if (escape_offset < (int)strlen(str) &&   /* check that we won't overflow */
                    !strncmp(str_before_token, escape_str, strlen(escape_str))) {  /* compare previous characters to escape character */

                is_escaped = 1;
            }

        }

        for (int i = 0; i < n; i++) {  /* begin checking for matching tokens */
            if (!strncmp(str_with_offset, delims[i], strlen(delims[i]))) {
                if (csp_esc[i] && is_escaped)
                    is_escaped = 1;
                else if (is_escaped && !csp_esc[i])
                    is_escaped = 0;
                matching = 1;
                delim_len = strlen(delims[i]);
                break;
            }
        }

        if (matching && !is_escaped) {
            int token_start = (int)i;
            int token_end = i + delim_len;
            if (current_starts + 1 > allocated) {
                allocated += 128;
                starts = realloc(starts, sizeof(int) * allocated);
                ends = realloc(ends, sizeof(int) * allocated);

                if (starts == NULL || ends == NULL) {
                    rc = 1;
                    goto end;
                }
            }

            starts[current_starts++] = token_start;
            ends[current_ends++] = token_end;
        }
    }

    /* begin adding tokens to char **tokens */

    int current_token_begin = 0;
    for (int i = 0; i < current_starts; i++) {  /* iterate through tokens */
        int start = starts[i];
        int end = ends[i];

        if (current_token_begin < start) {  /* don't add blank tokens */
            tokens[current_token] = malloc(sizeof(char) * (start - current_token_begin + 2));
            if (tokens[current_token] == NULL) {
                rc = 1;
                goto end;
            }

            int k = 0;
            int rem_esc = 0;  /* remaining characters of escape_str, used to ignore escape str in final tokens */
            for (int j = current_token_begin; j < start; j++) {
                if (!strncmp(str + j, escape_str, strlen(escape_str)))
                    rem_esc = (int)strlen(escape_str);

                if (rem_esc == 0)
                    tokens[current_token][k++] = str[j];
                else
                    rem_esc--;
            }
            tokens[current_token][k] = '\0';
            current_token++;
        }

        /* add deliminator as a new token */
        if (delims_as_tokens) {
            tokens[current_token] = malloc(sizeof(char) * (end - start + 2));
            if (tokens[current_token] == NULL) {
                rc = 1;
                goto end;
            }

            int k = 0;
            for (int j = start; j < end; j++) {
                tokens[current_token][k++] = str[j];    
            }
            tokens[current_token][k] = '\0';

            current_token++;
        }

        current_token_begin = end;
    }

    if (current_token_begin < (int)strlen(str)) {  /* don't add the last token if it is blank */
        /* allocate space for last token */
        tokens[current_token] = malloc(sizeof(char) * (strlen(str) - current_token_begin + 2));
        if (tokens[current_token] == NULL) {
            rc = 1;
            goto end;
        }

        /* add last token */
        int k = 0;
        for (size_t i = current_token_begin; i < strlen(str); i++)
            tokens[current_token][k++] = str[i];
        tokens[current_token][k] = '\0';
        current_token++;
    }

end:
    *dest = tokens;
    *dest_ntokens = current_token;

    free(starts);
    free(ends);
    return rc;
}

#endif
