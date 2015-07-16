#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

float hue(float r, float g, float b)
{
    r /= 255, g /= 255, b /= 255;
	float min = MIN(r, MIN(g, b));
	float max = MAX(r, MAX(g, b));
    float h = (max + min) / 2;
    float s = (max + min) / 2;
    float l = (max + min) / 2;

    if (max == min) {
        h = s = 0; // achromatic
    }
    else{
        float d = max - min;
        s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
        if (max == r) {
            h = (g - b) / d + (g < b ? 6 : 0);
        }
    	else if (max == g) {
            h = (b - r) / d + 2;
    	}
    	else {
            h = (r - g) / d + 4;
    	}
        h /= 6;
    }

    return h;
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