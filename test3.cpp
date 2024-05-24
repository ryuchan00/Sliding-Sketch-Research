#include <iostream>
#include <map>
#include <unordered_map>

struct ArrayHasher {
    std::size_t operator()(const std::array<int, 3>& a) const {
        std::size_t h = 0;

        for (auto e : a) {
            h ^= std::hash<int>{}(e) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

using namespace std;

typedef std::array<const unsigned char, 4> str;

std::map<str, int> dictionary;

int main() {
    const unsigned char s[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    const unsigned char s2[] = {0xFF, 0xFF, 0xFF, 0x11};

    // キーの配列を設定
    // dictionary[{s[0], s[1], s[2], s[3]}] = 2;
    dictionary[{s[0], s[1], s[2], s[3]}]++;

    // 出力される
    std::cout << dictionary[{s[0], s[1], s[2], s[3]}] << std::endl;
    // 出力されない
    std::cout << dictionary[{s2[0], s2[1], s2[2], s2[3]}] << std::endl;

    return 0;
}
