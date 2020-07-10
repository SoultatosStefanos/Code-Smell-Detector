// uvalue_untyped.cpp
// untyped (dynamic) value, general purpose.
// A. Savidis, 2017

#include "uobject_untyped.h"
#include "uvalue_untyped.h"

//---------------------------------------------------------------

namespace untyped {
	
//---------------------------------------------------------------

Value::Value (void) 
	{ value.type = Type::Undefined; }

Value::Value (const Value& other) 
	{ InitialiseFrom(other, true); }

Value::Value (Value&& other)  {
    InitialiseFrom(other, false);
    other.Nullify();
}

Value::~Value() 
	{ Clear(); }

const Value& Value::operator[](const Value& key) const
    { return ToObject().Get(key); }

//---------------------------------------------------------------

bool Value::operator==(const Value& other) const {
	
	if (this == &other)
		return true;
	else
    if (value.type != other.value.type)
        return false;
	else
		switch (value.type) {

			case Type::Undefined:
				return true;

			case Type::Number:
				return value.numberValue == other.value.numberValue;

			case Type::Boolean:
				return value.booleanValue == other.value.booleanValue;

			case Type::Reference:
				return value.referenceValue == other.value.referenceValue;

			case Type::String:
				return *DPTR(value.stringValue) == *DPTR(other.value.stringValue);

			case Type::Object:
				return *DPTR(value.objectValue) == *DPTR(other.value.objectValue);

			case Type::Function:
				return	value.functionValue.tag == other.value.functionValue.tag &&
						value.functionValue.tag != UVALUE_UNTYPED_FUNCTION_TAG_ALWAYS_UNIQUE;

			default: DASSERT(false); return false;
		}
}

//---------------------------------------------------------------

void Value::Undefine (void) {
   Clear();
   value.type = Type::Undefined;
 }

Value& Value::FromNumber (double number) {
    Clear();
    value.type = Type::Number;
    value.numberValue = number;
	return *this;
}

//---------------------------------------------------------------

Value& Value::FromString (const std::string& from) {
    Clear();
    value.type = Type::String;
    value.stringValue = DNEWCLASS(std::string, (from));
	return *this;
}

//---------------------------------------------------------------

Value& Value::FromBoolean (bool boolean) {
    Clear();
    value.type = Type::Boolean;
    value.booleanValue = boolean;
	return *this;
}

//---------------------------------------------------------------

Value& Value::FromReference (void* reference) {
    Clear();
    value.type = Type::Reference;
    value.referenceValue = reference;
	return *this;
}

//---------------------------------------------------------------

Value& Value::FromConstReference (const void* reference) {
    Clear();
    value.type = Type::ConstReference;
    value.constReferenceValue = reference;
	return *this;
}

//---------------------------------------------------------------

Value& Value::FromObject (const Object& object) {
	if (!IsObject() || &ToObject() != &object) {
		Clear();
		value.type = Type::Object;
		value.objectValue = DNEWCLASS(Object, (object));
	}
	return *this;
}

//---------------------------------------------------------------

Object& Value::FromObject (void) {
	if (!IsObject()) {
		Clear();
		value.type = Type::Object;
	}
	return *DPTR(value.objectValue = DNEW(Object));
}

//---------------------------------------------------------------

double Value::ToNumber (void) const {
    DASSERT(value.type == Type::Number);
    return value.numberValue;
}

//---------------------------------------------------------------

const std::string& Value::ToString (void) const {
    DASSERT(value.type == Type::String);
    return *DPTR(value.stringValue);
}

//---------------------------------------------------------------

bool Value::ToBoolean (void) const {
    DASSERT(value.type == Type::Boolean);
    return value.booleanValue;
}

//---------------------------------------------------------------

void* Value::ToReference (void) const {
    DASSERT(value.type == Type::Reference);
    return value.referenceValue;
}

//---------------------------------------------------------------

const void* Value::ToConstReference (void) const {
    DASSERT(value.type == Type::ConstReference);
    return value.constReferenceValue;
}

//---------------------------------------------------------------

Object& Value::ToObject (void) const {
    DASSERT(value.type == Type::Object);
    return *DPTR(value.objectValue);
}

//---------------------------------------------------------------

const Value::Function& Value::ToFunction (void) const {
	DASSERT(value.type == Type::Function);
    return *DPTR(value.functionValue.f);
}

util_ui64 Value::ToFunctionTag (void) const {
	DASSERT(value.type == Type::Function);
    return value.functionValue.tag;
}

//---------------------------------------------------------------

void Value::InitialiseFrom (const Value& other, bool copy) {

    switch (value.type = other.value.type) {	// mind the assignment

        case Type::Undefined:
            return;

        case Type::Number:
            value.numberValue = other.value.numberValue;
            return;

        case Type::Boolean:
            value.booleanValue = other.value.booleanValue;
            return;

        case Type::Reference:
            value.referenceValue = other.value.referenceValue;
            return;

        case Type::ConstReference:
            value.constReferenceValue = other.value.constReferenceValue;
            return;

		case Type::String:
            value.stringValue = copy ? DNEWCLASS(std::string, (*DPTR(other.value.stringValue))) : other.value.stringValue;
            return;

        case Type::Object:
            value.objectValue = copy ? DNEWCLASS(Object, (*DPTR(other.value.objectValue))) : other.value.objectValue;
            return;

		case Type::Function:
            value.functionValue.f = copy ? DNEWCLASS(Function, (*DPTR(other.value.functionValue.f))) : other.value.functionValue.f;
			value.functionValue.tag = other.value.functionValue.tag;
            return;
    }
}

//---------------------------------------------------------------

void Value::Nullify (void) {

    switch (value.type) {

        case Type::Undefined:
            return;

        case Type::Number:
            value.numberValue = 0;
            break;

        case Type::Boolean:
            value.booleanValue = false;
            break;

        case Type::Reference:
            value.referenceValue = nullptr;
            break;

        case Type::ConstReference:
            value.constReferenceValue = nullptr;
            break;

		case Type::String:
            value.stringValue = nullptr;
            break;

        case Type::Object:
            value.objectValue = nullptr;
            break;

		case Type::Function:
            value.functionValue.f = nullptr;
			value.functionValue.tag = UVALUE_UNTYPED_FUNCTION_TAG_ALWAYS_UNIQUE;
            break;
    }

    value.type = Type::Undefined;
}

//---------------------------------------------------------------

void Value::Clear (void) {

    switch (value.type) {

        case Type::Undefined:
        case Type::Number:
        case Type::Boolean:
        case Type::Reference:
        case Type::ConstReference:
			break;

        case Type::String:
            DDELETE(value.stringValue);
            break;

        case Type::Object:
            DDELETE(value.objectValue);
            break;

		case Type::Function:
            DDELETE(value.functionValue.f);
            break;
    }

    value.type = Type::Undefined;
}

//---------------------------------------------------------------

std::size_t ValueHash::operator()(const Value& value) const {

    switch (value.value.type) {

        case Value::Type::Number:
            return std::hash<double>{}(value.value.numberValue);

        case Value::Type::Boolean:
            return std::hash<bool>{}(value.value.booleanValue);

        case Value::Type::Reference:
            return std::hash<util_ui64>{}(reinterpret_cast<util_ui64>(value.value.referenceValue));

        case Value::Type::ConstReference:
            return std::hash<util_ui64>{}(reinterpret_cast<util_ui64>(value.value.constReferenceValue));

		case Value::Type::String:
            return std::hash<std::string>{}(*DPTR(value.value.stringValue));

        case Value::Type::Object:
            return std::hash<util_ui64>{}(reinterpret_cast<util_ui64>(value.value.objectValue));

        case Value::Type::Function:
            return std::hash<util_ui64>{}(reinterpret_cast<util_ui64>(value.value.functionValue.f));

		case Value::Type::Undefined:
            return 0;

		default: DASSERT(false); return 0;
    }
}

//---------------------------------------------------------------

} // untyped
