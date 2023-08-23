/****************************************
Designed by WHD in LinAn.2023
链表，以循环链表实现，保存的指针指向最后一个元素，
在最后一个元素和第一个元素之间还有一个尾后元素，尾后元素data_指针为空，
链表的尾删时间为n 
****************************************/
#ifndef DLIST_HPP
#define DLIST_HPP

#include "DType.h"
#include <stdexcept>

template <class T>
class DListNode
{
public:
	DListNode(T* data = nullptr, DListNode<T>* next = nullptr)
		: data_(data)
		, next_(next) {}

	~DListNode()
	{
		if (nullptr != data_)
			delete data_;
	}

	bool operator==(const DListNode<T>& node)
	{
		return (data_ == node.data_) && (next_ == node.next_);
	}

	T* data_;
	DListNode<T>* next_;
};

template <class T>
class DListIterator
{
public:
	DListIterator(DListNode<T>* node = nullptr)
		: node_(node) {}

	DListIterator(const DListIterator<T>& it)
		: node_(it->node_) {}

	DListIterator& operator++()
	{
		node_ = node_->next_;
		return *this;
	}

	DListIterator& operator++(int)
	{
		DListIterator ret = *this;
		++*this;
		return ret;
	}

	DListIterator& operator+=(DSizeType i)
	{
		while (i > 0)
		{
			node_ = node_->next_;
			--i;
		}
		return *this;
	}

	DListIterator operator+(DSizeType i)
	{
		DListIterator it = *this;
		it += i;
		return it;
	}

	T& operator*()
	{
		return *(node_->data_);
	}

	T* operator->()
	{
		return node_->data_;
	}

	DListIterator& operator=(const DListIterator<T>& it)
	{
        node_ = it.node_;
		return *this;
	}

	bool operator==(const DListIterator<T>& it)
	{
		return node_ == it.node_;
	}

	bool operator!=(const DListIterator<T>& it)
	{
		return !(*this == it);
	}

private:
	DListNode<T>* node_;
};

template <class T>
class DList
{
	friend class DPrinter;
public:
    typedef DListNode<T> Node;
	typedef DListIterator<T> iterator;

	DList();
	DList(const DList& list);
	DList(const std::initializer_list<T>& list);
	~DList() { clear(); }

	DList<T>& append(const T& data) { return pushBack(data); }
	DList<T>& append(const DList<T>& list);
	DList<T>& append(const std::initializer_list<T>& list);
	T& at(DSizeType index) { return *(nodeAt(index)->data_); }
	const T& at(DSizeType index) const { return *(nodeAt(index)->data_); }
	T& back() { return last(); }
	const T& back() const { return last(); }
	iterator begin() const { return iterator(last_->next_->next_); }
	void clear();
	bool contains(const T& data) const;
	DSizeType count(const T& data) const;
	iterator end() const { return iterator(last_->next_); }
	T& first();
	const T& first() const;
	T& front() { return first(); }
	const T& front() const { return first(); }
	DSizeType indexOf(const T& data) const;
	DList<DSizeType> indexListOf(const T& data) const;
	void insert(DSizeType index, const T& data);
	bool isEmpty() const { return 0 == size(); }
	T& last();
	const T& last() const;
	DSizeType length() const { return size(); }
	void move(DSizeType index1, DSizeType index2);
	DSizeType nIndexOf(DSizeType n, const T& data) const;
	Node* nodeAt(DSizeType index);
	Node* nodeAt(DSizeType index) const;
	T& operator[](DSizeType index) { return at(index); }
	const T& operator[](DSizeType index) const { return at(index); }
	DList<T>& operator=(const DList<T>& list);
	DList<T>& operator=(const std::initializer_list<T>& list);
	bool operator==(const DList<T>& list) const;
	bool operator!=(const DList<T>& list) const { return !(*this == list); }
	DList<T>& operator+=(const T& data) { return append(data); }
	DList<T> operator+(const T& data) const;
	DList<T>& operator+=(const DList<T>& list) { return append(list); }
	DList<T> operator+(const DList<T>& list) const;
	DList<T>& operator+=(const std::initializer_list<T>& list) { return append(list); }
	DList<T> operator+(const std::initializer_list<T>& list) const;
	DList<T>& operator*=(DSizeType num);
	DList<T> operator*(DSizeType num) const;
	T popBack();
	T popFront();
	DList<T>& pushBack(const T& data);
	DList<T>& pushFront(const T& data);
	void removeAll(const T& data);
	void removeAt(DSizeType index);
	void removeFirst(const T& data);
	DList<T>& replace(DSizeType index, const T& data);
	DList<T>& replace(const T& old_data, const T& new_data) { replaceFirst(old_data, new_data); }
	DList<T>& replaceAll(const T& old_data, const T& new_data);
	DList<T>& replaceFirst(const T& old_data, const T& new_data);
	void reverse();
	DSizeType size() const { return size_; }

private:
	Node* last_;
	DSizeType size_;
};

/********************************* 实现 *********************************/

template <class T>
DList<T>::DList()
	: last_(new Node())
	, size_(0)
{
	last_->next_ = last_;  // 尾后节点
}

template <class T>
DList<T>::DList(const DList<T>& list)
	: last_(new Node())
{
	last_->next_ = last_;
	*this = list;
}

template <class T>
DList<T>::DList(const std::initializer_list<T>& list)
	: last_(new Node())
{
	last_->next_ = last_;
	*this = list;
}

template <class T>
DList<T>& DList<T>::append(const DList<T>& list)
{
	iterator last_it = iterator(last_);
	for (iterator it = begin(); it != last_it; ++it)
		append(*it);
	append(*last_it);
	size_ += list.size_;
	return *this;
}

template <class T>
DList<T>& DList<T>::append(const std::initializer_list<T>& list)
{
	for (const T& data : list)
		append(data);
	return *this;
}

template <class T>
void DList<T>::clear()
{
	if (!isEmpty())
	{
		Node* node = last_->next_->next_;
		last_ = last_->next_;
		last_->next_ = last_;
		Node* temp;
		while (node != last_)
		{
			temp = node;
			node = node->next_;
			delete temp;
		}
		size_ = 0;
	}
}

template <class T>
bool DList<T>::contains(const T& data) const
{
	for (const T& d : *this)
	{
		if (d == data)
			return true;
	}

	return false;
}

template <class T>
DSizeType DList<T>::count(const T& data) const
{
	DSizeType ret = 0;
	for (const T& d : *this)
	{
		if (d == data)
			++ret;
	}

	return ret;
}

template <class T>
T& DList<T>::first()
{
	if (isEmpty())
		throw std::runtime_error("DList is empty!");
	else
		return *(last_->next_->next_->data_);
}

template <class T>
const T& DList<T>::first() const
{
	if (isEmpty())
		throw std::runtime_error("DList is empty!");
	else
		return *(last_->next_->next_->data_);
}

template <class T>
DSizeType DList<T>::indexOf(const T& data) const
{
	DSizeType index = 0;
	for (const T& d : *this)
	{
		if (d == data)
			break;
		++index;
	}

	return index;
}

template <class T>
DList<DSizeType> DList<T>::indexListOf(const T& data) const
{
	DList<DSizeType> index_list;
	DSizeType index = 0;
	for (const T& d : *this)
	{
		if (d == data)
			index_list.pushBack(index);
		++index;
	}

	return index_list;
}

template <class T>
void DList<T>::insert(DSizeType index, const T& data)
{
	if (index > size())
		throw std::runtime_error("index is bigger than size!");

	Node* front_node = last_->next_;
	for (DSizeType i = 0; i < index; ++i)
	{
		front_node = front_node->next_;
	}
	front_node->next_ = new Node(new T(data), front_node->next_);

	if (isEmpty())
		last_ = last_->next_;

	++size_;
}

template <class T>
T& DList<T>::last()
{
	if (isEmpty())
		throw std::runtime_error("DList is empty!");
	else
		return *(last_->data_);
}

template <class T>
const T& DList<T>::last() const
{
	if (isEmpty())
		throw std::runtime_error("DList is empty!");
	else
		return *(last_->data_);
}

template <class T>
void DList<T>::move(DSizeType index1, DSizeType index2)
{
	if (index1 >= size() || index2 >= size())
		throw std::runtime_error("index is bigger than size!");

	Node* prev1 = last_->next_;
	Node* prev2 = last_->next_;

	while (index1 > 0 || index2 > 0)
	{
		if (index1 > 0)
		{
			prev1 = prev1->next_;
			--index1;
		}

		if (index2 > 0)
		{
			prev2 = prev2->next_;
			--index2;
		}
	}

	Node* node1 = prev1->next_;
	Node* node2 = prev2->next_;
	prev1->next_ = node2;
	prev2->next_ = node1;

	Node* temp = node1->next_;
	node1->next_ = node2->next_;
	node2->next_ = temp;
}

template <class T>
DSizeType DList<T>::nIndexOf(DSizeType n, const T& data) const
{
	DSizeType index = 0;
	for (const T& d : *this)
	{
		if (d == data)
		{
			if (n == 0)
				break;
			--n;
		}
		++index;
	}

	return index;
}

template <class T>
DListNode<T>* DList<T>::nodeAt(DSizeType index)
{
	if (index >= size())
		throw std::runtime_error("index is bigger than size!");

	Node* node = last_->next_;
	for (DSizeType i = 0; i < index; ++i)
		node = node->next_;

	return node->next_;
}

template <class T>
DListNode<T>* DList<T>::nodeAt(DSizeType index) const
{
	if (index >= size())
		throw std::runtime_error("index is bigger than size!");

	Node* node = last_->next_;
	for (DSizeType i = 0; i < index; ++i)
		node = node->next_;

	return node->next_;
}

template <class T>
DList<T>& DList<T>::operator=(const DList<T>& list)
{
	if (!isEmpty())
		clear();
	size_ = list.size_;

	for (const T& data : list)
		pushBack(data);
	
	return *this;
}

template <class T>
DList<T>& DList<T>::operator=(const std::initializer_list<T>& list)
{
	if (!isEmpty())
		clear();
	size_ = list.size();

	for (const T& data : list)
		pushBack(data);
	
	return *this;
}

template <class T>
bool DList<T>::operator==(const DList<T>& list) const
{
	if (size() != list.size())
		return false;

	iterator it1 = begin();
	iterator it2 = list.begin();
	while (it1 != end())
	{
		if (*it1 != *it2)
			return false;
		++it1;
		++it2;
	}
	return true;
}

template <class T>
DList<T> DList<T>::operator+(const T& data) const
{
	DList<T> list(*this);
	list += data;
	return list;
}

template <class T>
DList<T> DList<T>::operator+(const DList<T>& list) const
{
	DList<T> ret_list(*this);
	ret_list += list;
	return ret_list;
}

template <class T>
DList<T> DList<T>::operator+(const std::initializer_list<T>& list) const
{
	DList<T> ret_list(*this);
	ret_list += list;
	return ret_list;
}

template <class T>
DList<T>& DList<T>::operator*=(DSizeType num)
{
	if (num == 0)
		clear();
	else
	{
		DList<T> temp(*this);
		while (--num > 0)
		{
			*this += temp;
		}
	}
	
	return *this;
}

template <class T>
DList<T> DList<T>::operator*(DSizeType num) const
{
	DList<T> ret_list(*this);
	ret_list *= num;
	return ret_list;
}

template <class T>
T DList<T>::popBack()
{
	if (isEmpty())
		throw std::runtime_error("DList is empty!");

	T data = *(last_->data_);

	Node* second_last = last_->next_;
	while (second_last->next_ != last_)
		second_last = second_last->next_;

	second_last->next_ = last_->next_;
	delete last_;
	last_ = second_last;

	--size_;
	return data;
}

template <class T>
T DList<T>::popFront()
{
	if (isEmpty())
		throw std::runtime_error("DList is empty!");

	T data = *(last_->next_->next_->data_);
	if (1 == size())
	{
		delete last_;
		last_ = nullptr;
	}
	else
	{
		Node* node = last_->next_->next_;
		last_->next_->next_ = last_->next_->next_->next_;
		delete node;
	}

	--size_;
	return data;
}

template <class T>
DList<T>& DList<T>::pushBack(const T& data)
{
	last_->next_ = new Node(new T(data), last_->next_);
	last_ = last_->next_;
	++size_;
	return *this;
}

template <class T>
DList<T>& DList<T>::pushFront(const T& data)
{
	last_->next_->next_ = new Node(new T(data), last_->next_->next_);
	if (isEmpty())
		last_ = last_->next_;

	++size_;
	return *this;
}

template <class T>
void DList<T>::removeAll(const T& data)
{
	if (isEmpty())
		return;

	Node* node = last_->next_;
	while (node->next_ != last_->next_)
	{
		if (*(node->next_->data_) == data)
		{
			Node* next_node = node->next_->next_;
			delete node->next_;
			node->next_ = next_node;
		}
		node = node->next_;
	}
}

template <class T>
void DList<T>::removeAt(DSizeType index)
{
	if (index >= size())
		throw std::runtime_error("index is bigger than size!");

	Node* node = last_->next_;
	for (DSizeType i = 0; i < index; ++i)
		node = node->next_;

	Node* delete_node = node->next_;
	node->next_ = delete_node->next_;
	delete delete_node;
}

template <class T>
void DList<T>::removeFirst(const T& data)
{
	if (isEmpty())
		return;

	Node* node = last_->next_;
	while (node->next_ != last_->next_)
	{
		if (*(node->next_->data_) == data)
		{
			Node* next_node = node->next_->next_;
			delete node->next_;
			node->next_ = next_node;
			break;
		}
		node = node->next_;
	}
}

template <class T>
DList<T>& DList<T>::replace(DSizeType index, const T& data)
{
	at(index) = data;
	return *this;
}

template <class T>
DList<T>& DList<T>::replaceAll(const T& old_data, const T& new_data)
{
	for (iterator it = begin(); it != end(); ++it)
	{
		if (*it == old_data)
		{
			*it = new_data;
		}
	}
}

template <class T>
DList<T>& DList<T>::replaceFirst(const T& old_data, const T& new_data)
{
	for (iterator it = begin(); it != end(); ++it)
	{
		if (*it == old_data)
		{
			*it = new_data;
			break;
		}
	}
}

template <class T>
void DList<T>::reverse()
{
	Node* end_node = last_->next_;
	Node* prev_node = end_node;
	Node* node = last_->next_->next_;
	last_ = last_->next_->next_;

	Node* temp = nullptr;
	while (node != end_node)
	{
		temp = node;
		node = node->next_;
		temp->next_ = prev_node;
		prev_node = temp;
	}
	end_node->next_ = prev_node;
}

#endif  // DLIST_HPP
