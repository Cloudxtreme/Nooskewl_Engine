#include <cstdio>
#include <vector>
#include <string>
#include <cstring>

int main(void)
{
	std::vector<std::string> lines;
	char buf[1000];

	while (fgets(buf, 1000, stdin)) {
		if (buf[strlen(buf)-1] == '\n' || buf[strlen(buf)-1] == 'r') buf[strlen(buf)-1] = 0;
		if (buf[strlen(buf)-1] == '\n' || buf[strlen(buf)-1] == 'r') buf[strlen(buf)-1] = 0;
		lines.push_back(buf);
	}

	int i = 0;
	while (i < (int)lines.size()) {
		std::string s = lines[i];
		printf("%s\n", s.c_str());
		bool changed = false;
		for (size_t j = 0; j < lines.size();) {
			if (j == i) {
				j++;
				continue;
			}
			std::string s2 = lines[j];
			if (s == s2) {
				lines.erase(lines.begin() + j);
				changed = true;
			}
			else {
				j++;
			}
		}
		i++;
	}
}