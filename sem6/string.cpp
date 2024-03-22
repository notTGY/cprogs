#include <iostream>
#include <cstring>

class String {
public:
    String(const char* str = nullptr) : data(nullptr), size(0), capacity(0) {
        if (str != nullptr) {
            size = strlen(str);
            capacity = size;
            data = new char[capacity + 1];
            strcpy(data, str);
        }
    }

    String(size_t n, char c) : data(new char[n + 1]), size(n), capacity(n) {
        for (size_t i = 0; i < size; ++i) {
            data[i] = c;
        }
        data[size] = '\0';
    }

    String(const String& other) : String(other.data) {}

    String& operator=(String other) {
        swap(*this, other);
        return *this;
    }

    bool operator==(const String& other) const {
        return size == other.size && memcmp(data, other.data, size) == 0;
    }

    char& operator[](size_t index) {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    const char& operator[](size_t index) const {
        if (index >= size) {
            throw std::out_of_range("Index out of range");
        }
        return data[index];
    }

    size_t length() const {
        return size;
    }

    void push_back(char c) {
        if (size == capacity) {
            reserve(2 * capacity + 2);
        }
        data[size++] = c;
        data[size] = '\0';
    }

    void pop_back() {
        if (!empty()) {
            --size;
            data[size] = '\0';
        }
    }

    char& front() {
        if (empty()) {
            throw std::runtime_error("Empty string");
        }
        return data[0];
    }

    const char& front() const {
        if (empty()) {
            throw std::runtime_error("Empty string");
        }
        return data[0];
    }

    char& back() {
        if (empty()) {
            throw std::runtime_error("Empty string");
        }
        return data[size - 1];
    }

    const char& back() const {
        if (empty()) {
            throw std::runtime_error("Empty string");
        }
        return data[size - 1];
    }

    String& operator+=(char c) {
        push_back(c);
        return *this;
    }

    String& operator+=(const String& other) {
        for (size_t i = 0; i < other.size; ++i) {
            push_back(other[i]);
        }
        return *this;
    }

    friend String operator+(String lhs, char rhs) {
        lhs += rhs;
        return lhs;
    }

    friend String operator+(char lhs, String rhs) {
        rhs += lhs;
        return rhs;
    }

    friend String operator+(String lhs, const String& rhs) {
        lhs += rhs;
        return lhs;
    }

    size_t find(const String& substring) const {
        for (size_t i = 0; i <= size - substring.size; ++i) {
            if (memcmp(&data[i], substring.data, substring.size) == 0) {
                return i;
            }
        }
        return size;
    }

    size_t rfind(const String& substring) const {
        for (size_t i = size - substring.size; i > 0; --i) {
            if (memcmp(&data[i], substring.data, substring.size) == 0) {
                return i;
            }
        }
        return size;
    }

    String substr(size_t start, size_t count = -1) const {
        if (start >= size || count > size - start) {
            throw std::out_of_range("Invalid arguments");
        }
        if (count == -1) {
            count = size - start;
        }
        String result(count, ' ');
        memcpy(result.data, &data[start], count);
        return result;
    }

    bool empty() const {
        return size == 0;
    }

    void clear() {
        delete[] data;
        data = nullptr;
        size = capacity = 0;
    }

    ~String() {
        clear();
    }

private:
    void reserve(size_t newCapacity) {
        if (newCapacity <= capacity) {
            return;
        }
        char* newData = new char[newCapacity + 1];
        if (data) {
          memcpy(newData, data, size + 1);
          delete[] data;
        }
        data = newData;
        capacity = newCapacity;
    }

    char* data;
    size_t size;
    size_t capacity;

    friend void swap(String& first, String& second) noexcept {
        using std::swap;
        swap(first.data, second.data);
        swap(first.size, second.size);
        swap(first.capacity, second.capacity);
    }
};

std::ostream& operator<<(std::ostream& os, const String& s) {
    for (size_t i = 0; i < s.length(); ++i) {
        os << s[i];
    }
    return os;
}

std::istream& operator>>(std::istream& is, String& s) {
    s.clear();
    char c;
    while (is >> c) {
        s.push_back(c);
    }
    return is;
}