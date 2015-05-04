#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdlib.h>

#include "Log.h"

template<typename T>
class Buffer
{
public:
	Buffer(size_t capacity = 1024) : capacity(capacity) , size(0) {
		if (capacity == 0) {
			data = nullptr;
			return;
		}

		data = reinterpret_cast<T*>(malloc(capacity * sizeof(T)));
		if (!data) {
			THROW_SYSTEM_ERROR();
		}
	}

	~Buffer() {
		if (data) {
			free(data);
		}
	}

	size_t size() const {
		return size;
	}

	size_t capacity() const {
		return capacity;
	}

	void setCapacity(size_t _capacity) {
		capacity = capacity;
		errno = 0;
		data = reinterpret_cast<T*>(
			realloc(data, capacity * sizeof(T)));
		if (errno) {
			THROW_SYSTEM_ERROR();
		}
	}

	void append(const T *src, size_t _size) {
		if (capacity < size + _size) {
			setCapacity(capacity + size);
		}

		if (src >= data && src < data + capacity) {
			_E("src and dst do ovelap")
		}
		memcpy(data + size, src, _size);

		size += _size;
	}

	void drain(size_t n) {
		if (n == 0)
			return;

		if (n > size) {
			THROW("not enough data");
		}

		size_t count = size - n;
		T *ptr1 = data;
		T *ptr2 = data + n;

		while (count) {
			size_t count2 = std::min(n, count);

			memcpy(ptr1, ptr2, count2);
			ptr1 = ptr2;
			ptr2 += count2;
			count -= count2;
		}

		size -= n;
	}

	const T* data() {
		return data;
	}

private:
	T* data;
	size_t capacity;
	size_t size;
};

#endif /* __BUFFER_H__ */
