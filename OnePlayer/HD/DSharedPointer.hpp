#ifndef DSHAREDPOINTER_HPP
#define DSHAREDPOINTER_HPP

#include "DType.h"

template <class T>
class DSharedPointer
{
	friend class DPrinter;
public:
	explicit DSharedPointer(T* data);
	DSharedPointer(const DSharedPointer<T>& pointer);
	~DSharedPointer();

	DSizeType useCount() const { return *count_; }
	DSharedPointer<T>& operator=(const DSharedPointer<T>& pointer);
	bool operator==(const DSharedPointer<T>& pointer);
	bool operator!=(const DSharedPointer<T>& pointer) { return !(*this == pointer); }
	T& operator*() { return *data_; }
	T& operator*() const { return *data_; }
	T* operator->() { return data_; }

private:
	DSizeType* count_;
	T* data_;
};

/********************************* й╣ож *********************************/

template <class T>
DSharedPointer<T>::DSharedPointer(T* data)
	: data_(data)
	, count_(new DSizeType(1))
{

}

template <class T>
DSharedPointer<T>::DSharedPointer(const DSharedPointer<T>& pointer)
	: data_(pointer.data_)
	, count_(pointer.count_)
{
	++*count_;
}

template <class T>
DSharedPointer<T>::~DSharedPointer()
{
	if (--*count_ == 0)
	{
		if (nullptr != data_)
			delete data_;
		delete count_;
	}
}

template <class T>
DSharedPointer<T>& DSharedPointer<T>::operator=(const DSharedPointer<T>& pointer)
{
	++*pointer.count_;
	if (nullptr != count_)
	{
		if (--*count_ == 0)
		{
			if (nullptr != data_)
				delete data_;
		}
	}
	data_ = pointer.data_;
	count_ = pointer.count_;
	return *this;
}

template <class T>
bool DSharedPointer<T>::operator==(const DSharedPointer<T>& pointer)
{
	return data_ == pointer.data_;
}

#endif  // DSHAREDPOINTER_HPP
