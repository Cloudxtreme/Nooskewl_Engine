#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

float hue(int r, int g, int b)
{
	float R = (float)r / 255.0f;
	float G = (float)g / 255.0f;
	float B = (float)b / 255.0f;
	float min, max;
	min = MIN(R, MIN(G, B));
	max = MAX(R, MAX(G, B));

	if (max == R) {
		return (G-B)/(max-min);
	}
	else if (max == G) {
		return 2.0f + (B-R)/(max-min);
	}
	else {
		return 4.0f + (R-G)/(max-min);
	}
}

bool compare(std::string a, std::string b)
{
	int r_a, g_a, b_a;
	int r_b, g_b, b_b;

	sscanf(a.c_str(), "rgb: %d %d %d", &r_a, &g_a, &b_a);
	sscanf(b.c_str(), "rgb: %d %d %d", &r_b, &g_b, &b_b);

	return hue(r_a, g_a, b_a) < hue(r_b, g_b, b_b);
}

int main(void)
{
	std::vector<std::string> lines;
	char buf[1000];

	while (fgets(buf, 1000, stdin)) {
		lines.push_back(buf);
	}

	std::sort(lines.begin(), lines.end(), compare);

	for (size_t i = 0; i < lines.size(); i++) {
		printf("%s", lines[i].c_str());
	}
}