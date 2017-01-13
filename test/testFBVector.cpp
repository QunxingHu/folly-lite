#include <iostream>
#include <folly/FBVector.h>
#include <folly/FBString.h>

int main()
{
    std::cout << folly::fbstring("using jemalloc: ") <<  std::boolalpha << folly::usingJEMalloc() << std::endl;
    folly::fbvector<int> vec;
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }
    folly::fbstring str = "Hello World";
    folly::fbvector<char> toAppend({ '0', '1', '2', '3', '4' });
    std::copy(toAppend.begin(), toAppend.end(), std::back_inserter(str));
    std::cout << str << std::endl;
    return 0;
 }