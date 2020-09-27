#ifndef DA2_VECTOR_H
#define DA2_VECTOR_H

#include <iostream>

template <typename T>
class TVector {
public:
    TVector() = default;

    TVector(size_t newSize) {
        data = new T[newSize];
        size = newSize;
        capacity = newSize;
    }

    TVector(size_t newSize, T value) {
        data = new T[newSize];
        for (size_t i = 0; i < newSize; ++i) {
            data[i] = value;
        }
        size = newSize;
        capacity = newSize;
    }

    TVector(const TVector& other) {
        if (other.size != 0) {
            data = new T[other.size];
            for (size_t i = 0; i < other.size; ++i) {
                data[i] = other.data[i];
            }
            size = other.size;
            capacity = other.size;
        }
    }

    TVector& operator = (const TVector& other) {
        TVector temp = other;
        std::swap(data, temp.data);
        std::swap(size, temp.size);
        std::swap(capacity, temp.capacity);
        return *this;
    }

    ~TVector() {
        delete[] data;
    }

    size_t Size() const {
        return size;
    }

    const T* Data() const {
        return data;
    }

    void PushBack(T element) {
        if (size == capacity) {
            size_t newCapacity = capacity == 0 ? 1 : capacity  * 2;
            T* newData = new T[newCapacity];
            for (size_t i = 0; i < size; ++i) {
                newData[i] = data[i];
            }
            capacity = newCapacity;
            delete[] data;
            data = newData;
        }
        data[size++] = std::move(element);

    }

    const T& operator[] (size_t index) const {
        if (index >= size) {
            throw std::out_of_range("");
        }
        return data[index];
    }

    T& operator[] (size_t index) {
        if (index >= size) {
            throw std::out_of_range("");
        }
        return data[index];
    }

private:
    T* data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
};


#endif //DA2_VECTOR_H
