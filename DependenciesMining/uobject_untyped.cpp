// uobject_untyped.cpp
// untyped (dynamic) object, general purpose.
// A. Savidis, 2017

#include "uobject_untyped.h"
#include "uvalue_untyped.h"

//---------------------------------------------------------------

namespace untyped {

Object::Object(const Object& other) {
    values.insert(
		other.values.begin(), 
		other.values.end()
	);
}

//---------------------------------------------------------------

bool Object::operator==(const Object& other) const {
	if (this == &other)
		return true;
	else
		return	values.size() == other.values.size() && 
				std::equal(
					values.begin(), 
					values.end(), 
					other.values.begin()
				);
}

const Value& Object::operator[](const Value& key) const 
	{ return Get(key); }

//---------------------------------------------------------------

bool Object::In (const Value& key) const 
	{ return values.find(key) != values.end(); }

const Value& Object::Get (const Value& key) const {
    auto i = values.find(key);	
    DASSERT(i != values.end());
    return i->second;
}

//---------------------------------------------------------------

Object& Object::Set (const Value& key, const Value& value) {

    auto i = values.find(key);
    if (i != values.end())
        i->second = value;
    else
        values.insert({key, value});

    return *this;
}

//---------------------------------------------------------------

Object& Object::RemoveValue (const Value& key) {

    auto i = values.find(key);
	DASSERT(i != values.end());
	values.erase(i);

    return *this;
}

//---------------------------------------------------------------

} // untyped
