// uobject_untyped.h
// untyped (dynamic) object, general purpose.
// A. Savidis, 2017

#ifndef UOBJECT_UNTYPED_H
#define UOBJECT_UNTYPED_H

#include "uvalue_untyped.h"
#include <unordered_map>

//---------------------------------------------------------------

namespace untyped {

class UTILLIB_CLASS Value;
struct UTILLIB_CLASS ValueHash {
    std::size_t operator()(const Value& value) const;
};

class UTILLIB_CLASS Object {

	public:
    Object (void) = default;
    Object (const Object& other);
	Object (Object&& other) : values(other.values){}
	~Object() { Clear(); }

	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(Object)
	UOVERLOADED_ASSIGN_VIA_MOVE_CONSTRUCTOR(Object)

    bool			operator== (const Object& other) const;
    bool			operator!= (const Object& other) const
						{ return !operator==(other); }

	bool			In (const Value& key) const;
    const Value&	Get (const Value& key) const;
	const Value&	operator[](const Value& key) const; 
    Object&			Set (const Value& key, const Value& value);
    Object&			RemoveValue (const Value& key);
	void			Clear (void) 
						{ values.clear(); }
	util_ui32		GetTotal (void) const
						{ return (util_ui32) values.size(); }
	template <typename Tfunc>
	void			ForEach (const Tfunc& f) const {
						for (auto& i : values)
							f(i.first, i.second);
					}

	//---------------------------------------------------------------

	private:
    using ValueMap = std::unordered_map<const Value, Value, ValueHash>;
    ValueMap values;
};

} // untyped

#endif	// Do not add stuff beyond this point.
