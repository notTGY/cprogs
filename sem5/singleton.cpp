#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>

class A {
public:
    static A* instance; // Singleton instance
    static int64_t creationTime; // Time of creation in microseconds
private:
    A() {
      A::instance = nullptr;
      A::creationTime = 0;
    } // Private constructor to prevent instances from being created

public:
    static A* getA() {
        if (A::instance == nullptr) {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            A::creationTime = (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
            A::instance = new A();
        }
        return A::instance;
    }

    int64_t time() {
        return A::creationTime;
    }
};