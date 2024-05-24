#include <unordered_map>
#include <map>
#include <iostream>

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

typedef std::array<unsigned int, 4> alphabet;

std::map<alphabet, std::string> dictionary;

int main() {
    array<int, 3> key = {0, 1, 2};

    unordered_map<array<int, 3>, int, ArrayHasher> test;


    test[key] = 2;

    std::cout << test[key] << std:: endl;

    dictionary[{1, 0, 3 , 4}] = "hello";

    std::cout << dictionary[{1, 0, 3 , 4}] << std::endl;


    return 0;
}