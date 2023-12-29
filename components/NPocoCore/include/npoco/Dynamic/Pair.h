/*	
	Pair.h - Definition of the Pair class.
*/


#ifndef Core_Pair_INCLUDED
#define Core_Pair_INCLUDED


#include "../Core.h"
#include "../Dynamic/Var.h"
#include "../Dynamic/VarHolder.h"
#include <utility>


namespace Poco {
namespace Dynamic {


template <typename K>
class Pair
	/// Pair allows to define a pair of values.
{
public:
	using Data = typename std::pair<K, Var>;

	Pair(): _data()
		/// Creates an empty Pair
	{
	}

	Pair(const Pair& other): _data(other._data)
		/// Creates the Pair from another pair.
	{
	}

	Pair(const Data& val): _data(val)
		/// Creates the Pair from the given value.
	{
	}

	template <typename T>
	Pair(const std::pair<K, T>& val): _data(std::make_pair(val.first, val.second))
		/// Creates Pair form standard pair.
	{
	}

	template <typename T>
	Pair(const K& first, const T& second): _data(std::make_pair(first, second))
		/// Creates pair from two values.
	{
	}

	virtual ~Pair()
		/// Destroys the Pair.
	{
	}

	Pair& swap(Pair& other)
		/// Swaps the content of the two Pairs.
	{
		std::swap(_data, other._data);
		return *this;
	}

	Pair& operator = (const Pair& other)
		/// Copy constructs Pair from another pair.
	{
		Pair(other).swap(*this);
		return *this;
	}

	inline const K& first() const
		/// Returns the first member of the pair.
	{
		return _data.first;
	}

	inline const Var& second() const
		/// Returns the second member of the pair.
	{
		return _data.second;
	}

	std::string toString()
	{
		std::string str;
		Var(*this).template convert<std::string>(str);
		return str;
	}

private:
	Data _data;
};


template <>
class VarHolderImpl<Pair<std::string>>: public VarHolder
{
public:
	VarHolderImpl(const Pair<std::string>& val): _val(val)
	{
	}

	~VarHolderImpl()
	{
	}
	
	const std::type_info& type() const
	{
		return typeid(Pair<std::string>);
	}

	bool convert(Int8& val) const {
		//throw BadCastException("Cannot cast Pair type to Int8");
		return false;
	}

	bool convert(Int16& val) const {
		//throw BadCastException("Cannot cast Pair type to Int16");
		return false;
	}
	
	bool convert(Int32& val) const {
		//throw BadCastException("Cannot cast Pair type to Int32");
		return false;
	}

	bool convert(Int64& val) const {
		//throw BadCastException("Cannot cast Pair type to Int64");
		return false;
	}

	bool convert(UInt8& val) const {
		//throw BadCastException("Cannot cast Pair type to UInt8");
		return false;
	}

	bool convert(UInt16& val) const {
		//throw BadCastException("Cannot cast Pair type to UInt16");
		return false;
	}
	
	bool convert(UInt32& val) const {
		//throw BadCastException("Cannot cast Pair type to UInt32");
		return false;
	}

	bool convert(UInt64& val) const {
		//throw BadCastException("Cannot cast Pair type to UInt64");
		return false;
	}

	bool convert(bool& val) const {
		//throw BadCastException("Cannot cast Pair type to bool");
		return false;
	}

	bool convert(float& val) const {
		//throw BadCastException("Cannot cast Pair type to float");
		return false;
	}

	bool convert(double& val) const {
		//throw BadCastException("Cannot cast Pair type to double");
		return false;
	}

	bool convert(char& val) const {
		//throw BadCastException("Cannot cast Pair type to char");
		return false;
	}

	bool convert(std::string& val) const {
		// Serialize in JSON format: equals an object
		// JSON format definition: { string ':' value } string:value pair n-times, sep. by ','
		val.append("{ ");
		Var key(_val.first());
		Impl::appendJSONKey(val, key);
		val.append(" : ");
		Impl::appendJSONValue(val, _val.second());
		val.append(" }");
		
		return true;
	}

	bool convert(Poco::DateTime&) const {
		//throw BadCastException("Pair -> Poco::DateTime");
		return false;
	}

	bool convert(Poco::LocalDateTime&) const {
		//throw BadCastException("Pair -> Poco::LocalDateTime");
		return false;
	}

	bool convert(Poco::Timestamp&) const {
		//throw BadCastException("Pair -> Poco::Timestamp");
		return false;
	}

	VarHolder* clone(Placeholder<VarHolder>* pVarHolder = 0) const
	{
		return cloneHolder(pVarHolder, _val);
	}
	
	const Pair<std::string>& value() const
	{
		return _val;
	}

	bool isArray() const
	{
		return false;
	}

	bool isStruct() const
	{
		return false;
	}

	bool isInteger() const
	{
		return false;
	}

	bool isSigned() const
	{
		return false;
	}

	bool isNumeric() const
	{
		return false;
	}

	bool isString() const
	{
		return false;
	}

private:
	Pair<std::string> _val;
};


template <>
class VarHolderImpl<Pair<int>>: public VarHolder
{
public:
	VarHolderImpl(const Pair<int>& val): _val(val)
	{
	}

	~VarHolderImpl()
	{
	}
	
	const std::type_info& type() const {
		return typeid(Pair<int>);
	}

	bool convert(Int8& val) const {
		//throw BadCastException("Cannot cast Pair type to Int8");
		return false;
	}

	bool convert(Int16& val) const {
		//throw BadCastException("Cannot cast Pair type to Int16");
		return false;
	}
	
	bool convert(Int32& val) const {
		//throw BadCastException("Cannot cast Pair type to Int32");
		return false;
	}

	bool convert(Int64& val) const {
		//throw BadCastException("Cannot cast Pair type to Int64");
		return false;
	}

	bool convert(UInt8& val) const {
		//throw BadCastException("Cannot cast Pair type to UInt8");
		return false;
	}

	bool convert(UInt16& val) const {
		//throw BadCastException("Cannot cast Pair type to UInt16");
		return false;
	}
	
	bool convert(UInt32& val) const {
		//throw BadCastException("Cannot cast Pair type to UInt32");
		return false;
	}

	bool convert(UInt64& val) const	{
		//throw BadCastException("Cannot cast Pair type to UInt64");
		return false;
	}

	bool convert(bool& val) const {
		//throw BadCastException("Cannot cast Pair type to bool");
		return false;
	}

	bool convert(float& val) const {
		//throw BadCastException("Cannot cast Pair type to float");
		return false;
	}

	bool convert(double& val) const {
		//throw BadCastException("Cannot cast Pair type to double");
		return false;
	}

	bool convert(char& val) const {
		//throw BadCastException("Cannot cast Pair type to char");
		return false;
	}

	bool convert(std::string& val) const {
		// Serialize in JSON format: equals an object
		// JSON format definition: { string ':' value } string:value pair n-times, sep. by ','
		val.append("{ ");
		Var key(_val.first());
		Impl::appendJSONKey(val, key);
		val.append(" : ");
		Impl::appendJSONValue(val, _val.second());
		val.append(" }");
		
		return true;
	}

	bool convert(Poco::DateTime&) const {
		//throw BadCastException("Pair -> Poco::DateTime");
		return false;
	}

	bool convert(Poco::LocalDateTime&) const {
		//throw BadCastException("Pair -> Poco::LocalDateTime");
		return false;
	}

	bool convert(Poco::Timestamp&) const {
		//throw BadCastException("Pair -> Poco::Timestamp");
		return false;
	}

	VarHolder* clone(Placeholder<VarHolder>* pVarHolder = 0) const
	{
		return cloneHolder(pVarHolder, _val);
	}
	
	const Pair<int>& value() const
	{
		return _val;
	}

	bool isArray() const
	{
		return false;
	}

	bool isStruct() const
	{
		return false;
	}

	bool isInteger() const
	{
		return false;
	}

	bool isSigned() const
	{
		return false;
	}

	bool isNumeric() const
	{
		return false;
	}

	bool isString() const
	{
		return false;
	}

private:
	Pair<int> _val;
};


} // namespace Dynamic


} // namespace Poco


#endif // Core_Pair_INCLUDED
