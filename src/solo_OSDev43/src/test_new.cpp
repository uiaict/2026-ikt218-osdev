extern "C" void terminal_write(const char*);
// overloaded new in c++ without having to rewrite everything to c++
extern "C" void test_new(void) {
    int* x = new int;

    if (x) {
        *x = 42;
        terminal_write("overloaded new works\n");
    }
}