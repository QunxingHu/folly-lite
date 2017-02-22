#include <iostream>
#include <folly/FBString.h>

using namespace std;

int main()
{
  folly::fbstring str1(10,'a');
  folly::fbstring str2(250,'b');
  folly::fbstring str3(300,'c');

  if(str1 > str2)
    cout << "str1 is small than b" << endl;
  else
    cout << "str2 is larger than a. Wrong." << endl;

  cout  << "str1 + str2 " << str1+str2 << endl;

  if(str1+str2 != str3)
    cout << "str1 + str2 != str3" << endl;
  else
    cout << "str1 + str2 == str3, Wrong." << endl;

  folly::fbstring strTmp = str1;
  if(strTmp.compare(str1))
    cout << "compare str1 is equal to strTmp." << endl;
  else
    cout << "compare wrong." << endl;

  strTmp += str2;
  if(strTmp == str1+str2)
    cout << "== operation pass." << endl;
  else
    cout << "Wrong in == operation." << endl;

  return 0;
}
