#include <templater/Variable.h>

using namespace GCM::templater;
using namespace GCM;

GCM_API Variable::Variable(): type(T_NULL) { }
GCM_API Variable::Variable(String value): type(T_STRING), otherValue(value.getObj()) { }
GCM_API Variable::Variable(uint32_t value): type(T_INT) { this->value.asInt = value; }
GCM_API Variable::Variable(int32_t value): type(T_INT) { this->value.asInt = value; }
GCM_API Variable::Variable(uint64_t value): type(T_INT64) { this->value.asInt64 = value; }
GCM_API Variable::Variable(int64_t value): type(T_INT64) { this->value.asInt64 = value; }
GCM_API Variable::Variable(double value): type(T_DOUBLE) { this->value.asDouble = value; }
GCM_API Variable::Variable(GC<HashTable> value): type(T_HASHTABLE), otherValue(value.getObj()) { }

GCM_API Variable::~Variable() {
	this->destructor();
}

void GCM_API Variable::destructor() {
	/*switch (this->type) {
		case T_STRING:
		case T_HASHTABLE:
			this->otherValue = NULL;
			break;

		default:
			break;
	}*/
}

WeakGC<Variable> GCM_API Variable::assign(String value) {
	this->destructor();
	this->type = T_STRING;
	this->otherValue = value.getObj();
	//GCM::util::Log::d("Variable", "String assign");
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(uint32_t value) {
	this->destructor();
	this->type = T_INT;
	this->value.asInt = value;
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(int32_t value) {
	this->destructor();
	this->type = T_INT;
	this->value.asInt = value;
	//GCM::util::Log::d("Variable", "int assign");
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(uint64_t value) {
	this->destructor();
	this->type = T_INT64;
	this->value.asInt64 = value;
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(int64_t value) {
	this->destructor();
	this->type = T_INT64;
	this->value.asInt64 = value;
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(double value) {
	this->destructor();
	this->type = T_DOUBLE;
	this->value.asDouble = value;
	//GCM::util::Log::d("Variable", "double assign");
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(GC<HashTable> value) {
	this->destructor();
	this->type = T_HASHTABLE;
	this->otherValue = value.getObj();
	//GCM::util::Log::d("Variable", "hashtable assign");
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(GC<Variable> value) {
	switch (value->type) {
		case T_STRING: this->assign(value->toString()); break;
		case T_INT: this->assign(value->toInt()); break;
		case T_INT64: this->assign(value->toInt64()); break;
		case T_DOUBLE: this->assign(value->toDouble()); break;
		case T_HASHTABLE: this->assign(value->toHashTable()); break;
		case T_NULL: this->destructor(); this->type = T_NULL; break;
	}

	return this;
}

WeakGC<Variable> GCM_API Variable::assign(String key, String value) {
	GC<Variable> out = this->access(key);
	out->assign(value);
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(String key, uint32_t value) {
	GC<Variable> out = this->access(key);
	out->assign(value);
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(String key, int32_t value) {
	GC<Variable> out = this->access(key);
	out->assign(value);
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(String key, uint64_t value) {
	GC<Variable> out = this->access(key);
	out->assign(value);
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(String key, int64_t value) {
	GC<Variable> out = this->access(key);
	out->assign(value);
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(String key, double value) {
	GC<Variable> out = this->access(key);
	out->assign(value);
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(String key, GC<HashTable> value) {
	GC<Variable> out = this->access(key);
	out->assign(value);
	return this;
}

WeakGC<Variable> GCM_API Variable::assign(String key, GC<Variable> value) {
	GC<Variable> out = this->access(key);
	out->assign(value);
	return this;
}

bool GCM_API Variable::operator==(Variable &compare) {
	switch (this->type) {
		case T_STRING: return this->toString() == compare.toString();
		case T_INT: return this->value.asInt == compare.toInt();
		case T_INT64: return this->value.asInt64 == compare.toInt64();
		case T_DOUBLE: return this->value.asDouble == compare.toDouble();
		case T_HASHTABLE: return this->toHashTable() == compare.toHashTable();
		default:
		case T_NULL: return false;
	}
}

bool GCM_API Variable::operator>(Variable &compare) {
	switch (this->type) {
		case T_STRING: return this->toString() > compare.toString();
		case T_INT: return this->value.asInt > compare.toInt();
		case T_INT64: return this->value.asInt64 > compare.toInt64();
		case T_DOUBLE: return this->value.asDouble > compare.toDouble();
		case T_HASHTABLE: return this->toHashTable() > compare.toHashTable();
		default:
		case T_NULL: return false;
	}
}

GCM_API Variable::operator String() {
	return this->toString();
}

GCM_API Variable::operator uint32_t() {
	return this->toInt();
}

GCM_API Variable::operator int32_t() {
	return this->toInt();
}

GCM_API Variable::operator uint64_t() {
	return this->toInt64();
}

GCM_API Variable::operator int64_t() {
	return this->toInt64();
}

GCM_API Variable::operator double() {
	return this->toDouble();
}

GCM_API Variable::operator GC<HashTable>() {
	return this->toHashTable();
}

GCM_API Variable::operator bool() {
	return !this->isNull() && this->toInt() != 0;
}

bool GCM_API Variable::isNull() {
	return this->type == T_NULL;
}

VarType GCM_API Variable::getType() {
	return this->type;
}

GC<Variable> GCM_API Variable::access(String index) {
	if (this->type != T_HASHTABLE) {
		//GCM::util::Log::d("Variable", "Variable is not hashtable, create HT.");
		this->assign(new HashTable());
	}

	HashTable::iterator i = this->toHashTable()->find(index);
	if (i != this->toHashTable()->end()) {
		return i->second;
	} else {
		//GCM::util::Log::d("Variable", "Create new index %s", index->c_str());
		return this->toHashTable()->assign(index, new Variable());
	}
}

String GCM_API Variable::toString() {
	switch (this->type) {
		case T_STRING: return (cString *)(this->otherValue.getObj());
		case T_INT: return String::format("%d", this->value.asInt);
		case T_DOUBLE: return String::format("%lf", this->value.asDouble);
		case T_HASHTABLE: return "HashTable";
		default:
		case T_NULL: return "";
	}
}

int32_t GCM_API Variable::toInt() {
	switch (this->type) {
		case T_STRING: return atoi(this->toString()->c_str());
		case T_INT: return this->value.asInt;
		case T_DOUBLE: return (int)this->value.asDouble;
		case T_HASHTABLE: return this->toHashTable()->size();
		default:
		case T_NULL: return 0;
	}
}

int64_t GCM_API Variable::toInt64() {
	switch (this->type) {
		case T_STRING: return atoi(this->toString()->c_str());
		case T_INT: return this->value.asInt;
		case T_INT64: return this->value.asInt64;
		case T_DOUBLE: return (int64_t)this->value.asDouble;
		case T_HASHTABLE: return this->toHashTable()->size();
		default:
		case T_NULL: return 0;
	}
}

double GCM_API Variable::toDouble() {
	switch (this->type) {
		case T_STRING: return atof(this->toString()->c_str());
		case T_INT: return (double)this->value.asInt;
		case T_INT64: return (double)this->value.asInt64;
		case T_DOUBLE: return this->value.asDouble;
		case T_HASHTABLE: return this->toHashTable()->size();
		default:
		case T_NULL: return 0;
	}
}

GC<HashTable> GCM_API Variable::toHashTable() {
	switch (this->type) {
		case T_STRING: {
			HashTable *arr = new HashTable();
			(*arr)["0"] = this;
			return arr;
		}

		case T_INT: {
			HashTable *arr = new HashTable();
			(*arr)["0"] = this;
			return arr;
		}

		case T_INT64: {
			HashTable *arr = new HashTable();
			(*arr)["0"] = this;
			return arr;
		}

		case T_DOUBLE: {
			HashTable *arr = new HashTable();
			(*arr)["0"] = this;
			return arr;
		}

		case T_HASHTABLE: {
			return (HashTable *)(this->otherValue.getObj());
		}

		default:
		case T_NULL: {
			return new HashTable();
		}
	}
}
