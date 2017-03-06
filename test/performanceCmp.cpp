#include <stdio.h>
#include <time.h>
#include <string>
#include <thread>
#include "folly/FBString.h"
#include <iostream>

using namespace folly;
using namespace std;

/*
g++ -DHAVE_CONFIG_H performanceCmp.cpp -o performanceCmp -pthread -std=c++11
depbase=`echo FBStringTest.o | sed 's|[^/]*$|.deps/&|;s|\.o$||'`;\
	g++ -DHAVE_CONFIG_H   -I../.. -pthread -I/usr/include -Igtest-1.7.0/include  -std=gnu++1y -g -O2 -MT FBStringTest.o -MD -MP -MF $depbase.Tpo -c -o FBStringTest.o FBStringTest.cpp &&\
	mv -f $depbase.Tpo $depbase.Po

g++ -DHAVE_CONFIG_H -pthread -std=gnu++1y -g -O2 performanceCmp.cpp -o performanceCmp
g++ -I../folly/ -pthread -std=c++11 -O2 \performanceCmp.cpp -o performanceCmp

/usr/bin/c++    -I/home/hqx/code/folly-lite/folly-lite/folly -I/home/hqx/code/folly-lite/folly-lite/jemalloc/include -I/home/hqx/code/folly-lite/folly-lite/jemalloc/jemalloc-4.2.0/include  -std=c++11 -lpthread -O2 -DHAVE_CONFIG_H -DLINUX -o CMakeFiles/performanceCmp.dir/test/performanceCmp.cpp.o -c /home/hqx/code/folly-lite/folly-lite/test/performanceCmp.cpp
/usr/bin/c++    -I/home/hqx/code/folly-lite/folly-lite/folly -I/home/hqx/code/folly-lite/folly-lite/jemalloc/include -I/home/hqx/code/folly-lite/folly-lite/jemalloc/jemalloc-4.2.0/include -DHAVE_CONFIG_H -DLINUX /home/hqx/code/folly-lite/folly-lite/test/performanceCmp.cpp -o out -std=c++11 -lpthread -O2
*/

void constructFBString(int size){
  struct timespec timeStart, timeEnd;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timeStart);
  for(int i=0; i<10000; i++)
  {
    fbstring fbstr1(size, 'a');
    fbstring fbstr2(size, 'b');
    fbstr1 += fbstr2;
    fbstr1.find('b');
  }
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timeEnd);
  double res = (timeEnd.tv_sec - timeStart.tv_sec)* 1000 + (timeEnd.tv_nsec-timeStart.tv_nsec)/ 1000000; //us
  printf("FBString thread time: %lf\n", res);
}

void constructStdString(int size){
  struct timespec timeStart, timeEnd;
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timeStart);
  for(int i=0; i<10000; i++){
      std::string str1(size,'a');
      std::string str2(size,'b');
      str1 += str2;
      str1.find("b");
  }
  clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timeEnd);
  double res = (timeEnd.tv_sec - timeStart.tv_sec)* 1000 + (timeEnd.tv_nsec-timeStart.tv_nsec)/ 1000000; //us
  printf("StdString thread time: %lf\n", res);
}

int main() {
  std::cout << folly::fbstring("using jemalloc: ") << std::boolalpha << folly::usingJEMalloc() << std::endl;
  clock_t c_start = clock();
  for(int i=0; i<100; i++){
    std::thread t3(constructFBString,500);
    t3.join();
  }
  clock_t c_end = clock();
  double fbTime = (double)(c_end - c_start) / (double) CLOCKS_PER_SEC;
  printf("FBString: CPU time: %lf s\n", fbTime);

  clock_t b_start = clock();
  for(int i=0; i<100; i++){
     std::thread t6(constructStdString, 500);
     t6.join();
  }
  clock_t b_end = clock();
  double stdTime = (double)(b_end - b_start) / (double) CLOCKS_PER_SEC;
  printf("StdString CPU time: %lf s\n", stdTime);
  return 0;
}


/*
#include <time.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <deque>
#include "folly/FBVector.h"
#include "folly/FBString.h"
#include <sys/types.h>
#include <sys/times.h>
#include <thread>


template <class Function>
void calcTime(folly::fbstring const& name, Function&& f)
{
    // calculate cpu time
    clock_t c_start = clock();
    f();
    clock_t c_end = clock();
    std::cout <<"CPU time: " << name << " " << (double)(c_end - c_start) / (double) CLOCKS_PER_SEC << " s\n";

    // calculate total time
    time_t t_start, t_end;
    time(&t_start);
    f();
    time(&t_end);
    std::cout <<"Total time: " << name << " " << (double)(t_end-t_start)<< " s\n";

    struct tms tms_start, tms_end;
    long tck = sysconf(_SC_CLK_TCK);
    c_start = times(&tms_start);
    f();
    c_end = times(&tms_end);
    std::cout <<"CPU time: " << name << " " << (double)(c_end - c_start) / (double) tck << " s\n";
    std::cout <<"user time: " << name << " " << (double)(tms_end.tms_utime - tms_start.tms_utime) / (double) tck << " s\n";
    std::cout <<"system time: " << name << " " << (double)(tms_end.tms_stime - tms_start.tms_stime) / (double) tck << " s\n";
    std::cout <<"child user time: " << name << " " << (double)(tms_end.tms_cutime - tms_start.tms_cutime) / (double) tck << " s\n";
    std::cout <<"child system time: " << name << " " << (double)(tms_end.tms_cstime - tms_start.tms_cstime) / (double) tck << " s\n";

    std::cout << std::endl;
}
void testThreadtime(folly::fbstring const& name){
  // calculate cpu time
  clock_t c_start = clock();
  folly::fbvector<int> vec;
  for (int i = 0; i < 10000000; ++i) {
      vec.push_back(i);
  }
  for (int i = 10000000 - 1; i > 0; --i) {
      vec[i - 1] += vec[i];
  }
  std::cout << "size: " << sizeof(vec) << std::endl;
  clock_t c_end = clock();
  std::cout <<"CPU time: " << name << " " << (double)(c_end - c_start) / (double) CLOCKS_PER_SEC << " s\n";
}
int main(void){
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
  std::thread t1(testThreadtime, "======thread : insert to fbvector: ");
  t1.join();

  calcTime("insert to fbvector: ", []{
      folly::fbvector<int> vec;
      for (int i = 0; i < N_TIMES; ++i) {
          vec.push_back(i);
      }
      for (int i = N_TIMES - 1; i > 0; --i) {
          vec[i - 1] += vec[i];
      }
      std::cout << "size: " << sizeof(vec) << std::endl;
  });

  calcTime("insert to std::vector", []{
      std::vector<int> vec;
      for (int i = 0; i < N_TIMES; ++i) {
          vec.push_back(i);
      }
      for (int i = N_TIMES - 1; i > 0; --i) {
          vec[i - 1] += vec[i];
      }
      std::cout << "size: " << sizeof(vec) << std::endl;
  });

  calcTime("append char[] to fbstring", []{
      folly::fbstring str;
      for (int i = 0; i < N_TIMES; ++i) {
          str += i%2?"hello":"world";
      }
      std::cout << "size: " << sizeof(str) << std::endl;
  });

  calcTime("append char[] to std::string", []{
      std::string str;
      for (int i = 0; i < N_TIMES; ++i) {
          str += i % 2 ? "hello" : "world";
      }
      std::cout << "size: " << sizeof(str) << std::endl;
  });

  calcTime("append fbstring to a fbstring", []{
      folly::fbstring str, hello = "hello", world = "world";
      for (int i = 0; i < N_TIMES; ++i) {
          str += i % 2 ? hello:world;
      }
      std::cout << "size: " << sizeof(str) << std::endl;
  });

  calcTime("append std::string to a std::string", []{
      std::string str, hello = "hello", world = "world";
      for (int i = 0; i < N_TIMES; ++i) {
          str += i % 2 ? hello : world;
      }
      std::cout << "size: " << sizeof(str) << std::endl;
  });

  calcTime("append char to a fbstring", []{
      folly::fbstring str;
      for (int i = 0; i < N_TIMES; ++i) {
          str += i % 2 ? 'A':'B';
      }
      std::cout << "size: " << sizeof(str) << std::endl;
  });

  calcTime("append char to a std::string", []{
      std::string str;
      for (int i = 0; i < N_TIMES; ++i) {
          str += i % 2 ? 'A' : 'B';
      }
      std::cout << "size: " << sizeof(str) << std::endl;
  });

  calcTime("construct many fbstrings", []{
      folly::fbvector<folly::fbstring> strs;
      for (int i = 0; i < N_TIMES; ++i) {
          strs.push_back(folly::fbstring(i%2?"hello":"world"));
      }
      std::cout << "size: " << sizeof(str) << std::endl;
  });

  calcTime("construct many std::strings", []{
      folly::fbvector<std::string> strs;
      for (int i = 0; i < N_TIMES; ++i) {
          strs.push_back(std::string(i % 2 ? "hello" : "world"));
      }
      std::cout << "size: " << sizeof(str) << std::endl;
  });

  //static_assert(!std::is_trivially_copyable<NoneTrivial>::value, "oops!");
  //static_assert(folly::IsRelocatable<NoneTrivial>::value, "oops!!");
  return !(vec.size() == 100);
}
*/
