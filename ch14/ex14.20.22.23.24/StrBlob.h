/***************************************************************************
 *  @file       main.cpp
 *  @author     Alan.W
 *  @date       01  JAN 2014
 *  @remark     10  JAN 2014    :   Rvalue reference version push_back added
 *              12  JAN 2014    :   ==  !=  are added.
 *              13  JAN 2014    :   <   >   <=    >= are added
 ***************************************************************************/
//!
//! Exercise 13.25:
//! Assume we want to define a version of StrBlob that acts like a value. Also
//! assume that we want to continue to use a shared_ptr so that our StrBlobPtr
//! class can still use a weak_ptr to the vector. Your revised class will need
//! a copy constructor and copy-assignment operator but will not need a destructor.
//! Explain what the copy constructor and copy-assignment operators must do.
//! Explain why the class does not need a destructor.
//!
//  Copy constructor and copy-assignment operator should dynamicly allocate memory
//  for its own , rather than share the object with the right hand operand.
//
//  StrBlob is using smart pointers which can be managed with synthesized destructor
//  If an object of StrBlob is out of scope, the desstructor for std::shared_ptr
//  will be called automaticaly to free the memory dynamicly allocated when the
//  use count goes to 0.
//!
//! Exercise 13.26:
//! Write your own version of the StrBlob class described in the previous exercise.
//!


#ifndef STRBLOB_H
#define STRBLOB_H

#include <vector>
#include <string>
#include <initializer_list>
#include <memory>
#include <stdexcept>


// forward declaration needed for friend declaration in StrBlob
class StrBlobPtr;

/**
 * @brief The StrBlob class
 */
class StrBlob
{
	friend class StrBlobPtr;
    friend bool operator ==(const StrBlob& lhs, const StrBlob& rhs);
    friend bool operator < (const StrBlob& lhs, const StrBlob& rhs);
    friend bool operator  >(const StrBlob& lhs, const StrBlob& rhs);
    friend bool operator <=(const StrBlob& lhs, const StrBlob& rhs);
    friend bool operator >=(const StrBlob& lhs, const StrBlob& rhs);
public:
    typedef std::vector<std::string>::size_type size_type;

    //! constructors
    StrBlob() : data(std::make_shared<std::vector<std::string>>()) { }
    StrBlob(std::initializer_list<std::string> il);

    //! copy constructor for ex13.26
    StrBlob(const StrBlob& sb);

    //! copy-assignment operator for ex13.26
    StrBlob&
    operator = (const StrBlob &sb);

    //! size operations
    size_type size() const { return data->size(); }
    bool empty() const { return data->empty(); }

    //! add and remove elements
    void push_back(const std::string &t)    { data->push_back(          t );}   //copy version
    void push_back(      std::string&&t)    { data->push_back(std::move(t));}   //move version

    void pop_back();

    //! element access
    std::string& front();
    std::string& back();

    //! interface to StrBlobPtr
    StrBlobPtr begin();
    StrBlobPtr end();
private:
    std::shared_ptr<std::vector<std::string>> data; 
    //! throws msg if data[i] isn't valid
    void check(size_type i, const std::string &msg) const;
};


/** @brief operators
 *  compare the std::vector<std::string> it points directly,
 *  using the relational operators defined fro std::vector
 *  in std.
 */
inline bool
operator ==(const StrBlob& lhs, const StrBlob& rhs)
{
    return *lhs.data == *rhs.data;
}

inline bool
operator !=(const StrBlob& lhs, const StrBlob& rhs)
{
    return !(lhs == rhs);
}

inline bool
operator < (const StrBlob& lhs, const StrBlob& rhs)
{
    return *lhs.data < *rhs.data;
}

inline bool
operator > (const StrBlob& lhs, const StrBlob& rhs)
{
    return *lhs.data > *rhs.data;
}

inline bool
operator <=(const StrBlob& lhs, const StrBlob& rhs)
{
    return (*lhs.data <= *rhs.data);
}

inline bool
operator >=(const StrBlob& lhs, const StrBlob& rhs)
{
    return (*lhs.data >= *rhs.data);
}


/** member function implementations
  */
//! constructor
inline
StrBlob::StrBlob(std::initializer_list<std::string> il): 
    data(std::make_shared<std::vector<std::string>>(il)) { }

//! copy constructor
inline
StrBlob::StrBlob(const StrBlob &sb) :
    data(std::make_shared<std::vector<std::string>>(*sb.data)){ }

//! copy-assignment operator for ex13.26
inline StrBlob&
StrBlob::operator =(const StrBlob &sb)
{
    auto p = std::make_shared<std::vector<std::string>>(*sb.data);
    std::swap(data,p);

    return *this;
}



/**
 * @brief The StrBlobPtr class
 */
class StrBlobPtr
{
    friend bool eq          (const StrBlobPtr&, const StrBlobPtr&);
    friend bool operator == (const StrBlobPtr&, const StrBlobPtr&);
public:
    StrBlobPtr(): curr(0) { }
    StrBlobPtr(StrBlob &a, size_t sz = 0) : wptr(a.data), curr(sz) { }

    //! newly overloaded why?
    StrBlobPtr(const StrBlob &a, const size_t sz = 0) : wptr(a.data), curr(sz) { }

    std::string& deref() const;
    StrBlobPtr& incr();       // prefix version
    StrBlobPtr& decr();       // prefix version
private:
    //! check returns a shared_ptr to the vector if the check succeeds
    std::shared_ptr<std::vector<std::string>> 
        check(std::size_t, const std::string&) const;

    //! store a weak_ptr, which means the underlying vector might be destroyed
    std::weak_ptr<std::vector<std::string>> wptr;  
    std::size_t curr;      // current position within the array
};

//! non member
//! ==
//! note :  no operator == is defined for weak_ptr in std.Hence, operator == cann't
//!         be used directly.
//! note :  comparison logic here is different from the eq() which is given by textbook
//!         in eq(), if l and r both are null then it returns true wiout caomparing curr
//!         in this operator, it returns true only when both pointer and curr are identical.
//!
inline bool
operator==(const StrBlobPtr& lhs, const StrBlobPtr& rhs)
{
    //! lock () returns shared_ptr when possible ,or nullptr otherwise.
    auto l = lhs.wptr.lock();
    auto r = rhs.wptr.lock();

    return (l == r) ? (lhs.curr == lhs.curr) : false;
}

inline bool
operator !=(const StrBlobPtr& lhs, const StrBlobPtr& rhs)
{
    return !(lhs == rhs);
}



inline
std::string& StrBlobPtr::deref() const
{
    auto p = check(curr, "dereference past end"); 
    return (*p)[curr];  // (*p) is the vector to which this object points
}

inline
std::shared_ptr<std::vector<std::string>> 
StrBlobPtr::check(std::size_t i, const std::string &msg) const
{
    auto ret = wptr.lock();   // is the vector still around?
    if (!ret)
        throw std::runtime_error("unbound StrBlobPtr");

    if (i >= ret->size()) 
        throw std::out_of_range(msg);
    return ret; // otherwise, return a shared_ptr to the vector
}

//! prefix: return a reference to the incremented object
inline
StrBlobPtr& StrBlobPtr::incr()
{
    // if curr already points past the end of the container, can't increment it
    check(curr, "increment past end of StrBlobPtr");
    ++curr;       // advance the current state
    return *this;
}

inline
StrBlobPtr& StrBlobPtr::decr()
{
    // if curr is zero, decrementing it will yield an invalid subscript
    --curr;       // move the current state back one element}
    check(-1, "decrement past begin of StrBlobPtr");
    return *this;
}

//! begin and end members for StrBlob
inline
StrBlobPtr
StrBlob::begin() 
{
	return StrBlobPtr(*this);
}

inline
StrBlobPtr
StrBlob::end() 
{
	auto ret = StrBlobPtr(*this, data->size());
    return ret; 
}

//! named equality operators for StrBlobPtr
inline
bool eq(const StrBlobPtr &lhs, const StrBlobPtr &rhs)
{
	auto l = lhs.wptr.lock(), r = rhs.wptr.lock();
	// if the underlying vector is the same 
	if (l == r) 
		// then they're equal if they're both null or 
		// if they point to the same element
		return (!r || lhs.curr == rhs.curr);
	else
		return false; // if they point to difference vectors, they're not equal
}

inline
bool neq(const StrBlobPtr &lhs, const StrBlobPtr &rhs)
{
	return !eq(lhs, rhs); 
}
#endif
