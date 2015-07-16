#include <cstdio>
#include <vector>
#include <string>

int main(void)
{
	std::vector<std::string> lines;
	char buf[1000];

	while (fgets(buf, 1000, stdin)) {
		lines.push_back(buf);
	}

	for (size_t i = 0; i < lines.size(); i++) {
		std::string s = lines[i];
		printf("%s", s.c_str());
		for (size_t j = i+1; j < lines.size();) {
			std::string s2 = lines[j];
			if (s == s2) {
				lines.erase(lines.begin() + j);
			}
			else {
				j++;
			}
		}
	}
}