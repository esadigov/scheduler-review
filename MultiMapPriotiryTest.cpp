#include <map>
#include <iostream>
#include <stdio.h>
#include <string>
#include <iterator>

std::multimap<uint32_t, std::string> mm = {
    {0, std::string("line 1")}, 
    {1, std::string("line 2")}, 
    {3, std::string("line 3")}
};


int main() 
{
    int n=0;
    for(std::multimap<uint32_t, std::string>::iterator it = mm.begin(); it != mm.end(); it++)
    {
        if(n == 1) {
            std::multimap<uint32_t, std::string>::node_type i = mm.extract(1); 
            if(!i.empty()) {
                i.key() = 2;
                mm.insert(std::move(i));
            } 
        }
        printf("Key: %u, value: %s\n", it->first, it->second.c_str());
        n++;
    }
}