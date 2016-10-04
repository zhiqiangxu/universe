#include <ucontext.h>  
#include <unistd.h>  
#include <iostream>
using namespace std;
  
int main(int argc, const char *argv[]){  
    ucontext_t context;  
  
    getcontext(&context);  
    cout << "Hello world" << endl;  
    sleep(1);  
    setcontext(&context);  
    return 0;  
}  
