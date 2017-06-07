#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

int json_ws(char *json)
{
	char *s = json;
	while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r')
		s++;
	return s - json;
}

int json_len(char *json)
{
	char *s = json;
	s += json_ws(s);
	if (*s == '"') {
		for (s++; *s && *s != '"'; s++)
			if (*s == '\\')
				s++;
		return *s == '"' ? s - json + 1 : s - json;
	}
	if (*s == '[') {
		s += json_ws(s + 1) + 1;
		while (*s && *s != ']') {
			s += json_len(s);
			s += json_ws(s);
			if (*s != ',')
				break;
			s += json_ws(s + 1) + 1;
		}
		return *s == ']' ? s - json + 1 : s - json;
	}
	if (*s == '{') {
		s += json_ws(s + 1) + 1;
		while (*s && *s != '}') {
			s += json_len(s);
			s += json_ws(s);
			if (*s != ':')
				break;
			s += json_len(s + 1) + 1;
			s += json_ws(s);
			if (*s != ',')
				break;
			s += json_ws(s + 1) + 1;
		}
		return *s == '}' ? s - json + 1 : s - json;
	}
	if (isalnum((unsigned char) *s)) {
		while (isalnum((unsigned char) *s))
			s++;
		return s - json;
	}
	return 0;
}

char *json_list(char *json, int key)
{
	char *s = json;
	int i = 0;
	if (*s != '[')
		return NULL;
	s += json_ws(s + 1) + 1;
	while (*s && *s != ']') {
		s += json_ws(s);
		if (i++ == key)
			return s;
		s += json_len(s);
		s += json_ws(s);
		if (*s != ',')
			return NULL;
		s += json_ws(s + 1) + 1;
	}
	return NULL;
}

int json_str(char *json, char *dst, int len)
{
	if (!json)
		return -1;
	json += json_ws(json);
	if (*json != '"')
		return -1;
	json++;
	while (*json != '"' && len > 1) {
		if (*json == '\\' && json[1])
			json++;
		*dst++ = *json++;
		len--;
	}
	*dst = '\0';
	return 0;
}

char *json_dict(char *json, char *key)
{
	char *s = json;
	char cur[256];
	if (*s != '{')
		return NULL;
	s += json_ws(s + 1) + 1;
	while (*s && *s != '}') {
		if (json_str(s, cur, sizeof(cur)))
			break;
		s += json_len(s);
		s += json_ws(s);
		if (*s != ':')
			return NULL;
		s += json_ws(s + 1) + 1;
		if (!strcmp(cur, key))
			return s;
		s += json_len(s);
		s += json_ws(s);
		if (*s != ',')
			break;
		s += json_ws(s + 1) + 1;
	}
	return NULL;
}
