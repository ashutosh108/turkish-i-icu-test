/*
Sample Makefile:
CFLAGS  = -ggdb -std=c99 -pedantic -Wall -Wextra -Werror \
          `pkg-config --cflags icu-uc icu-io` #-DU_DISABLE_RENAMING
LDLIBS = `pkg-config --libs icu-uc icu-io`

all: main

Sample usage:
$ rm -f ./main && make && LC_ALL=tr_TR.UTF-8 ./main
gives
> running under "en_US_POSIX" locale
> tolower("İ") == "i̇" (U+0069 U+0307)
on my machine
$ rm -f ./main && make && LC_ALL=C.UTF-8 ./main
gives
> running under "en_US_POSIX" locale
> tolower("İ") == "i̇" (U+0069 U+0307)
on my machine.

Verified to work with ICU 53.1, 69.1 and 70.1.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unicode/uchar.h>
#include <unicode/ustdio.h>
#include <unicode/ustring.h>

UChar * my_tolower(const char *src_utf8, int32_t *actual_len_ptr) {
	UErrorCode	status;
	int32_t		buf_len;
	UChar	   *buf;
	int32_t		src_len;
	UChar	   *src;
	int32_t		actual_len;

	status = U_ZERO_ERROR;
	u_strFromUTF8(NULL, 0, &src_len, src_utf8, strlen(src_utf8), &status);
	if (status != U_BUFFER_OVERFLOW_ERROR && U_FAILURE(status)) {
		fprintf(stderr, "u_strFromUTF8 error: %s\n", u_errorName(status));
		exit(1);
	}
	status = U_ZERO_ERROR;
	src = malloc(src_len * sizeof(*src));
	u_strFromUTF8(src, src_len, NULL, src_utf8, strlen(src_utf8), &status);
	if (U_FAILURE(status)) {
		fprintf(stderr, "u_strFromUTF8 error: %s", u_errorName(status));
		exit(1);
	}

	buf_len = src_len * 8;
	buf = malloc(buf_len * sizeof(*buf));
	status = U_ZERO_ERROR;
	actual_len = u_strToLower(buf, buf_len, src, src_len,
		NULL, // default locale
		&status);
	if (U_FAILURE(status)) {
		fprintf(stderr, "u_strToLower error: %s", u_errorName(status));
		exit(1);
	}
	if (actual_len > buf_len) {
		fprintf(stderr, "u_strToLower buffer truncated: capacity %d UChars, needed %d UChars\n", buf_len, actual_len);
	}
	if (actual_len_ptr)
		*actual_len_ptr = actual_len;
	return buf;
}

void demo(const char * in) {
	UFILE	   *out = u_get_stdout();
	UChar	   *str;
	int32_t		actual_len;

	str = my_tolower(in, &actual_len);
	u_fprintf(out, "tolower(\"%s\") == \"%S\" (", in, str);
	for (int32_t i=0; i<actual_len; i++) {
		printf("%sU+%04X", i==0?"":" ", str[i]);
	}
	printf(")\n");
	free(str);
}

int main() {
	printf("running under \"%s\" locale\n", uloc_getDefault());
	// demo("Qwe");
	// demo("ЙцуЁёЕе");
	demo("İ"); // this is U+0130, capital "I" with dot
	// demo("i");
	// demo("i̇");
}
