#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print(const char *s)
{
	while (*s) {
		if (*s != '\\')
			fputc(*s, stdout);
		s++;
	}
	fputc('\n', stdout);
}

int main(void)
{
	char buf[5000];

	while (fgets(buf, 5000, stdin)) {
		const char *p = buf;
		while (1) {
			p = strstr(p, "TRANSLATE(\"");
			if (p) {
				p += 11;
				const char *p2 = strstr(p, "\")END");
				if (p2) {
					int len = p2 - p;
					char s[5000];
					memcpy(s, p, len);
					s[len] = 0;
					print(s);
					p = p2 + 5;
				}
			}
			else {
				break;
			}
		}
	}
}