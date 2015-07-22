#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("Usage: %s <infile.txt> <outfile.gpl>\n", argv[0]);
		return 0;
	}

	FILE *in = fopen(argv[1], "r");
	if (in == NULL) {
		printf("Can't open %s\n", argv[1]);
		return 1;
	}
	FILE *out = fopen(argv[2], "w");
	if (out == NULL) {
		printf("Can't open %s\n", argv[2]);
		return 1;
	}

	fprintf(out, "GIMP Palette\n#\n");

	char buf[1000];

	while ((fgets(buf, 1000, in)) != NULL) {
		int r, g, b;
		if (sscanf(buf, "rgb: %d %d %d\n", &r, &g, &b) != 3) {
			printf("Corrupt input!\n");
		}
		fprintf(out, "%3d %3d %3d\tUntitled\n", r, g, b);
	}

	fclose(in);
	fclose(out);
}