/*
 * CONVERT DBLP SEARCH RESULTS TO REFER RECORDS
 *
 * Copyright (C) 2017 Ali Gholami Rudi <ali at rudi dot ir>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "json.h"

static char buf[1 << 20];

int main(void)
{
	int len = read(0, buf, sizeof(buf));
	char *res, *json;
	char tok[512];
	int i, j;
	buf[len] = '\0';
	json = buf;
	if (!(res = json_dict(json, "result")))
		return 1;
	if (!(res = json_dict(res, "hits")))
		return 1;
	if (!(res = json_dict(res, "hit")))
		return 1;
	for (i = 0; json_list(res, i); i++) {
		char *cur, *info, *authors;
		char type = 'I';	/* Journal, Book, Conference */
		cur = json_list(res, i);
		info = json_dict(cur, "info");
		if (!json_str(json_dict(info, "type"), tok, sizeof(tok)))
			type = tok[0];
		if (!json_str(json_dict(cur, "@id"), tok, sizeof(tok)))
			printf("%%L  %s\n", tok);
		authors = json_dict(info, "authors");
		if (authors) {
			char *author = json_dict(authors, "author");
			for (j = 0; json_list(author, j); j++)
				if (!json_str(json_list(author, j), tok, sizeof(tok)))
					printf("%%A  %s\n", tok);
			if (!json_list(author, 0))
				if (!json_str(author, tok, sizeof(tok)))
					printf("%%A  %s\n", tok);
		}
		if (!json_str(json_dict(info, "title"), tok, sizeof(tok)))
			printf("%%T  %s\n", tok);
		if (!json_str(json_dict(info, "venue"), tok, sizeof(tok)))
			printf("%%%c  %s\n", type == 'J' ? 'J' : 'B', tok);
		if (!json_str(json_dict(info, "volume"), tok, sizeof(tok)))
			printf("%%V  %s\n", tok);
		if (!json_str(json_dict(info, "number"), tok, sizeof(tok)))
			printf("%%N  %s\n", tok);
		if (!json_str(json_dict(info, "year"), tok, sizeof(tok)))
			printf("%%D  %s\n", tok);
		if (!json_str(json_dict(info, "pages"), tok, sizeof(tok)))
			printf("%%P  %s\n", tok);
		if (!json_str(json_dict(info, "url"), tok, sizeof(tok)))
			printf("%%O  %s\n", tok);
		printf("\n");
	}
	return 0;
}
