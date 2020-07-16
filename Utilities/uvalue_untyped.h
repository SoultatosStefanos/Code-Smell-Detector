// uvalue_untyped.h
// untyped (dynamic) value, general purpose.
// A. Savidis, 2017

#ifndef UVALUE_UNTYPED_H
#define UVALUE_UNTYPED_H

#include <unordered_map>
#include <functional>
#include <string>
#include <assert.h>

#define UVALUE_UNTYPED_FUNCTION_TAG_ALWAYS_UNIQUE	0XFFFFFFFF
#define DNEW(C)				new C
#define DNEWCLASS(C,ARGS)	new C(ARGS)
#define DDELETE(p)			delete (p)
#define DASSERT				assert
#define DPTR(p)				(p)

#define UTILLIB_CLASS
typedef int64_t			util_i64;
typedef uint64_t		util_ui64;
typedef unsigned int	util_ui32;		// Four bytes, unsigned.
typedef signed int		util_i32;		// Four bytes, signed.

static_assert(sizeof(uint64_t) <= sizeof(double));

#define	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(_class)						\
	const _class& operator=(const _class& rvalue) {							\
		if (this != &rvalue) {												\
			udestructor_invocation(this);									\
			new (this) _class(rvalue);										\
		}																	\
		return *this;														\
	}

#define	UOVERLOADED_ASSIGN_VIA_MOVE_CONSTRUCTOR(_class)						\
	const _class& operator=(_class&& xvalue) {								\
		udestructor_invocation(this);										\
		return *new (this) _class(xvalue);									\
	}

template <typename T> void udestructor_invocation(T* t)
{
	t->~T();
}

//---------------------------------------------------------------

namespace untyped {

class UTILLIB_CLASS Object;

class UTILLIB_CLASS Value {
    friend struct ValueHash;

	public:
	using Function = std::function<void(void)>;

    enum class Type : unsigned {
        Undefined,
        Number,
        String,
        Boolean,
        Reference,
		ConstReference,
        Object,
		Function
    };

	Value (void);
    Value (const Value& other);
    Value (Value&& other);
    ~Value();

	Value (Object& obj)				{ value.type = Type::Undefined; FromObject(obj);			}
	Value (double num)				{ value.type = Type::Undefined; FromNumber(num);			}
	Value (util_i64 num)			{ value.type = Type::Undefined; FromNumber((double) num);	}
	Value (const std::string& s)	{ value.type = Type::Undefined; FromString(s);				}
	Value (const char* s)			{ value.type = Type::Undefined; FromString(s);				}
	Value (bool val)				{ value.type = Type::Undefined; FromBoolean(val);			}
	Value (void* ref)				{ value.type = Type::Undefined; FromReference(ref);			}
	Value (const void* ref)			{ value.type = Type::Undefined; FromConstReference(ref);	}
	Value (const Function& f, util_ui64 tag = UVALUE_UNTYPED_FUNCTION_TAG_ALWAYS_UNIQUE)		
									{ value.type = Type::Undefined; FromFunction(f, tag);			}

	UOVERLOADED_ASSIGN_VIA_COPY_CONSTRUCTOR(Value)
	UOVERLOADED_ASSIGN_VIA_MOVE_CONSTRUCTOR(Value)

	Type					GetType (void) const 
								{ return value.type; }

	bool					IsUndefined (void) const		{ return value.type == Type::Undefined;			}
	bool					IsNumber (void) const			{ return value.type == Type::Number;			}
	bool					IsString (void) const			{ return value.type == Type::String;			}
	bool					IsBoolean (void) const			{ return value.type == Type::Boolean;			}
	bool					IsReference (void) const		{ return value.type == Type::Reference;			}
	bool					IsConstReference (void) const	{ return value.type == Type::ConstReference;	}
	bool					IsObject (void) const			{ return value.type == Type::Object;			}
	bool					IsFunction (void) const			{ return value.type == Type::Function;			}

    bool					operator==(const Value& other) const;
    bool					operator!=(const Value& other) const
								{ return !operator==(other); }
	void					operator()(void) 
								{ DASSERT(IsFunction()); ToFunction()(); }

    Value&					FromNumber (double number);
	Value&					FromNumber (util_i64 number) { FromNumber((double)number); }
	Value&					FromString (const std::string& s);
    Value&					FromBoolean (bool boolean);
    Value&					FromReference (void* reference);
    Value&					FromConstReference (const void* reference);
    Value&					FromObject (const Object& object);	// deep copy
    Object&					FromObject (void);
	void					Undefine (void);

	template <typename Tfunc>
	Value&					FromFunction (const Tfunc& f, util_ui64 tag = UVALUE_UNTYPED_FUNCTION_TAG_ALWAYS_UNIQUE) {
								Clear();
								value.type = Type::Function;
								value.functionValue.f = DNEWCLASS(Function, (f));
								value.functionValue.tag = tag;
								return *this;
							}

	double					ToNumber (void) const;
	float					ToNumber_float (void) const
								{ return (float) ToNumber(); }
	util_i32				ToNumber_i32 (void) const
								{ return (util_i32) ToNumber(); }
	util_ui32				ToNumber_ui32 (void) const
								{ return (util_ui32) ToNumber(); }
	util_i64				ToNumber_i64 (void) const
								{ return (util_i64) ToNumber(); }

    const std::string&		ToString (void) const;
    bool					ToBoolean (void) const;
    void*					ToReference (void) const;
    const void*				ToConstReference (void) const;
    Object&					ToObject (void) const;
	const Function&			ToFunction (void) const;
	util_ui64				ToFunctionTag (void) const;

    const Value&			operator[](const Value& key) const;
 
	//---------------------------------------------------------------

	private:
    void					InitialiseFrom (const Value& other, bool copy);
    void					Nullify (void);
    void					Clear (void);

    struct {

        Type type;

        union {
            double			numberValue;
            std::string*	stringValue;
            bool			booleanValue;
            void*			referenceValue;
            const void*		constReferenceValue;
            Object*			objectValue;
			struct {
				Function*	f;
				util_ui64	tag;
			} functionValue;
        };
    } value;
};

////////////////////////////////////////////////////////////////

template <typename T> 
struct value_getter_traits {};

template <> 
struct value_getter_traits<double> {
	double operator()(const Value& val) const { return val.ToNumber(); }
};

template <> 
struct value_getter_traits<float> {
	float operator()(const Value& val) const { return (float) val.ToNumber(); }
};

template <> 
struct value_getter_traits<bool> {
	bool operator()(const Value& val) const { return val.ToBoolean(); }
};

template <> 
struct value_getter_traits<std::string> {
	const std::string& operator()(const Value& val) const { return val.ToString(); }
};

////////////////////////////////////////////////////////////////

//---------------------------------------------------------------

} // untyped

#endif	// Do not add stuff beyond this point.
