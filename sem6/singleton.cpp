#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>

class A {
private:
    static A* instance;
    static int64_t creationTime;
    A() { }

public:
    static A* getA() {
        if (!instance) {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            creationTime = (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
            instance = new A();
        }
        return instance;
    }

    int64_t time() {
        return creationTime;
    }
};

A* A::instance = nullptr;
int64_t A::creationTime = 0;

main() {
    A* a = A::getA();
    a->time();
}