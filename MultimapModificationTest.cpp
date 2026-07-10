#include <map>
#include <iostream>

struct b
{
    int a;
    int b;
};

int main()
{
    std::multimap<int, b> mm;

    mm.emplace(1,b{1, 2});
    mm.emplace(1,b{1, 3});
    mm.emplace(1,b{1, 1});

    auto node = mm.extract(mm.begin());

    mm.emplace(1, node.mapped());

    for(auto it : mm) {
        std::cout << "key: " << it.first << "a, b: " << it.second.a << it.second.b << std::endl;
    }
}