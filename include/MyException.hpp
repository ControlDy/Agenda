#ifndef MY_EXCEPT
#define MY_EXCEPT

#include <stdexcept>
#include <string>
using namespace std;
class MyException {
 public:
 	MyException(string t_message): message(t_message) {}
 	string what() {return message;}
 private:
 	string message;
};

#endif
