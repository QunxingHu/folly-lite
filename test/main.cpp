#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <deque>
#include <folly/FBVector.h>
#include <folly/FBString.h>

#ifdef WIN32
#define OS_WINDOWS WIN32
#include <windows.h>
#endif
#ifdef LINUX
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#endif
#ifdef VXWORKS
#include "vxworks.h"
#include <tickLib.h>
#include <sysLib.h>
#endif

typedef unsigned long ULONG;

ULONG GetTickCount(void)
{
ULONG currentTime;
#ifdef WIN32
currentTime = GetTickCount();
#endif
#ifdef LINUX
struct timeval current;
gettimeofday(&current, NULL);
currentTime = current.tv_sec * 1000 + current.tv_usec/1000;
#endif
#ifdef OS_VXWORKS
ULONG timeSecond = tickGet() / sysClkRateGet();
ULONG timeMilsec = tickGet() % sysClkRateGet() * 1000 / sysClkRateGet();
currentTime = timeSecond * 1000 + timeMilsec;
#endif
return currentTime;
}

struct NoneTrivial
{
    NoneTrivial(int){}

    folly::fbvector<int> vec;
};

FOLLY_ASSUME_FBVECTOR_COMPATIBLE(NoneTrivial);

template <class Function>
void time_call(folly::fbstring const& name, Function&& f)
{
    int64_t begin = GetTickCount();
    f();
    int64_t elapsed = GetTickCount() - begin;
    std::cout << name << ": " << elapsed << " ticks\n";
}

int main()
{
    std::cout << folly::fbstring("using jemalloc: ") << std::boolalpha << folly::usingJEMalloc() << std::endl;
    folly::fbvector<int> vec;
    for (int i = 0; i < 100; ++i) {
        vec.push_back(i);
    }
    folly::fbstring str = "Hello World";
    folly::fbvector<char> toAppend({ '0', '1', '2', '3', '4' });
    std::copy(toAppend.begin(), toAppend.end(), std::back_inserter(str));
    std::cout << str << std::endl;

#define N_TIMES 10000000
    time_call("insert to fbvector: ", []{
        folly::fbvector<int> vec;
        for (int i = 0; i < N_TIMES; ++i) {
            vec.push_back(i);
        }
        for (int i = N_TIMES - 1; i > 0; --i) {
            vec[i - 1] += vec[i];
        }
    });

    time_call("insert to std::vector", []{
        std::vector<int> vec;
        for (int i = 0; i < N_TIMES; ++i) {
            vec.push_back(i);
        }
        for (int i = N_TIMES - 1; i > 0; --i) {
            vec[i - 1] += vec[i];
        }
    });

    time_call("append char[] to fbstring", []{
        folly::fbstring str;
        for (int i = 0; i < N_TIMES; ++i) {
            str += i%2?"hello":"world";
        }
    });

    time_call("append char[] to std::string", []{
        std::string str;
        for (int i = 0; i < N_TIMES; ++i) {
            str += i % 2 ? "hello" : "world";
        }
    });

    time_call("append fbstring to a fbstring", []{
        folly::fbstring str, hello = "hello", world = "world";
        for (int i = 0; i < N_TIMES; ++i) {
            str += i % 2 ? hello:world;
        }
    });

    time_call("append std::string to a std::string", []{
        std::string str, hello = "hello", world = "world";
        for (int i = 0; i < N_TIMES; ++i) {
            str += i % 2 ? hello : world;
        }
    });

    time_call("append char to a fbstring", []{
        folly::fbstring str;
        for (int i = 0; i < N_TIMES; ++i) {
            str += i % 2 ? 'A':'B';
        }
    });

    time_call("append char to a std::string", []{
        std::string str;
        for (int i = 0; i < N_TIMES; ++i) {
            str += i % 2 ? 'A' : 'B';
        }
    });

    time_call("construct many fbstrings", []{
        folly::fbvector<folly::fbstring> strs;
        for (int i = 0; i < N_TIMES; ++i) {
            strs.push_back(folly::fbstring(i%2?"hello":"world"));
        }
    });

    time_call("construct many std::strings", []{
        folly::fbvector<std::string> strs;
        for (int i = 0; i < N_TIMES; ++i) {
            strs.push_back(std::string(i % 2 ? "hello" : "world"));
        }
    });

    //static_assert(!std::is_trivially_copyable<NoneTrivial>::value, "oops!");
    //static_assert(folly::IsRelocatable<NoneTrivial>::value, "oops!!");
    return !(vec.size() == 100);
}
