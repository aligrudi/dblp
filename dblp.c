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
#include <string.h>
#include <unistd.h>
#include "json.h"

static int refno;

static int readauthor(char *author, char *dst, int len)
{
	if (!json_str(author, dst, len))
		return 0;
	if (json_dict(author, "text"))
		return json_str(json_dict(author, "text"), dst, len);
	return 1;
}

static void outref(char *info)
{
	char *authors;
	int type = 'I';
	char tok[512];
	int j;
	if (!json_str(json_dict(info, "type"), tok, sizeof(tok)))
		type = tok[0];
	printf("%%L  ref%03d\n", ++refno);
	authors = json_dict(info, "authors");
	if (authors) {
		char *author = json_dict(authors, "author");
		for (j = 0; json_list(author, j); j++)
			if (!readauthor(json_list(author, j), tok, sizeof(tok)))
				printf("%%A  %s\n", tok);
		if (!json_list(author, 0))
			if (!readauthor(author, tok, sizeof(tok)))
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
	if (!json_str(json_dict(info, "doi"), tok, sizeof(tok)))
		printf("%%O  https://doi.org/%s\n", tok);
	if (!json_str(json_dict(info, "ee"), tok, sizeof(tok)))
		printf("%%O  %s\n", tok);
	if (!json_str(json_dict(info, "url"), tok, sizeof(tok)))
		printf("%%O  %s\n", tok);
	printf("\n");
}

static void outbib(char *info)
{
	char *authors;
	int type = 'J';
	char tok[512];
	int j;
	if (!json_str(json_dict(info, "type"), tok, sizeof(tok)))
		type = tok[0];
	if (type == 'J' || type == 'I')
		printf("@journal");
	if (type == 'C')
		printf("@inproceedings");
	if (type == 'B')
		printf("@book");
	printf("{ref%03d,\n", ++refno);
	authors = json_dict(info, "authors");
	if (authors) {
		char *author = json_dict(authors, "author");
		printf("  author = {");
		for (j = 0; json_list(author, j); j++)
			if (!readauthor(json_list(author, j), tok, sizeof(tok)))
				printf("%s%s", j ? " and " : "", tok);
		if (!json_list(author, 0))
			if (!readauthor(author, tok, sizeof(tok)))
				printf("%s", tok);
		printf("},\n");
	}
	if (!json_str(json_dict(info, "title"), tok, sizeof(tok))) {
		if (tok[0] && tok[strlen(tok) - 1] == '.')
			tok[strlen(tok) - 1] = '\0';
		printf("  title = {%s},\n", tok);
	}
	if (!json_str(json_dict(info, "venue"), tok, sizeof(tok)))
		printf("  %s = {%s},\n", type == 'J' ? "journal" : "booktitle", tok);
	if (!json_str(json_dict(info, "volume"), tok, sizeof(tok)))
		printf("  volume = {%s},\n", tok);
	if (!json_str(json_dict(info, "number"), tok, sizeof(tok)))
		printf("  number = {%s},\n", tok);
	if (!json_str(json_dict(info, "year"), tok, sizeof(tok)))
		printf("  year = {%s},\n", tok);
	if (!json_str(json_dict(info, "pages"), tok, sizeof(tok)))
		printf("  pages = {%s},\n", tok);
	if (!json_str(json_dict(info, "doi"), tok, sizeof(tok)))
		printf("  doi = {%s},\n", tok);
	printf("}\n\n");
}

static char buf[1 << 20];

int main(int argc, char *argv[])
{
	char *res, *json;
	int len;
	int tex = 0;
	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == 'b') {
			tex = 1;
		} else if (argv[i][0] == '-' && argv[i][1] == 'r') {
			tex = 0;
		} else if (argv[i][0] == '-' && argv[i][1] == 'h') {
			printf("usage: %s [options] <dblp.json >bib.ref\n\n", argv[0]);
			printf("options:\n");
			printf("  -r \t\t output for refer\n");
			printf("  -b \t\t output for bibtex\n");
			return 0;
		}
	}
	len = read(0, buf, sizeof(buf));
	buf[len] = '\0';
	json = buf;
	if (!(res = json_dict(json, "result")))
		return 1;
	if (!(res = json_dict(res, "hits")))
		return 1;
	if (!(res = json_dict(res, "hit")))
		return 1;
	for (i = 0; json_list(res, i); i++) {
		char *cur, *info;
		cur = json_list(res, i);
		info = json_dict(cur, "info");
		if (tex)
			outbib(info);
		else
			outref(info);
	}
	return 0;
}
