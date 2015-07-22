#include <stdio.h>

int main(int argc, char **argv)
{
	int num = atoi(argv[1]);
	FILE *in = fopen(argv[2], "r");

	char buf[5000];

	while (fgets(buf, 5000, in)) {
		if (buf[strlen(buf)-1] == '\r' || buf[strlen(buf)-1] == '\n') buf[strlen(buf)-1] = 0;
		if (buf[strlen(buf)-1] == '\r' || buf[strlen(buf)-1] == '\n') buf[strlen(buf)-1] = 0;
		char match[5000];
		snprintf(match, 5000, "TRANSLATE(\"%s\")END", buf);
		char sub[5000];
		snprintf(sub, 5000, "noo.translate(%d)", num++);
		FILE *out = fopen("__out__.txt", "w");
		FILE *in2 = fopen(argv[3], "r");
		char buf2[5000];
		while (fgets(buf2, 5000, in2)) {
			while (1) {
				const char *p = strstr(buf2, match);
				if (p) {
					char buf3[5000];
					int len = p - buf2;
					memcpy(buf3, buf2, len);
					memcpy(buf3+len, sub, strlen(sub));
					memcpy(buf3+len+strlen(sub), p+strlen(match), strlen(p+strlen(match)));
					buf3[len+strlen(sub)+strlen(p+strlen(match))] = 0;
					strcpy(buf2, buf3);
				}
				else {
					break;
				}
			}
			fprintf(out, "%s", buf2);
		}
		fclose(out);
		fclose(in2);
		sprintf(buf, "move %s %s", "__out__.txt", argv[3]);
		system(buf);
	}	
}
