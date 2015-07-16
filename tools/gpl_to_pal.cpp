#include <cstdio>

void write16(FILE *f, unsigned short s)
{
	fputc(s & 0xff, f);
	s >>= 8;
	fputc(s & 0xff, f);
}

void write32(FILE *f, unsigned short s)
{
	fputc(s & 0xff, f);
	s >>= 8;
	fputc(s & 0xff, f);
	s >>= 8;
	fputc(s & 0xff, f);
	s >>= 8;
	fputc(s & 0xff, f);
}

int main(int argc, char **argv)
{
	FILE *in = fopen(argv[1], "r");
	FILE *out = fopen(argv[2], "wb");
	fprintf(out, "%s", "RIFF");
	write32(out, 1044);
	fprintf(out, "%s", "PAL ");
	fprintf(out, "%s", "data");
	write32(out, 256*4+8);
	write16(out, 0x0300);
	write16(out, 256);

	int count = 0;
	char buf[1000];

	while (fgets(buf, 1000, in)) {
		int r, g, b;
		if (count < 256) {
			if (sscanf(buf, "%d %d %d ", &r, &g, &b) == 3) {
				count++;
				fputc(r, out);
				fputc(g, out);
				fputc(b, out);
				fputc(0, out);
			}
		}
	}

	for (; count < 256; count++)  {
		write32(out, 0);
	}

	write32(out, 0xd801);
}