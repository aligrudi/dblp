/* parsing json format */

int json_ws(char *json);
int json_len(char *json);
char *json_list(char *json, int key);
char *json_dict(char *json, char *key);
int json_str(char *json, char *dst, int len);
