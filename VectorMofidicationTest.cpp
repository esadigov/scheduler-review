#include <vector>
#include <iostream>
#include <map>

struct block
{
    int i;
};


std::vector<block> blocks = {{12},{34}};
std::multimap<int, block> q;

int main() 
{
    std::multimap<int, block>::node_type node(1, blocks.be);
    
    q.insert(std::move(node));

    std::cout << blocks.at(0).i << std::endl;

    std::cout << q.begin()->first << " " << q.begin()->second.i << std::endl;
}