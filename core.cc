#include "core.h"
#include <iterator>
#include <v8.h>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <iostream>
#include <sstream>
#include <iterator>
#include <numeric>
#include <sstream>

// -----------------------------------------------------------------------------------------
// --- POINT --------------------------------------------------------------------------------------

class Point {
public:
	Point(int x, int y) : x_(x), y_(y) { }
	int x_, y_;
};

v8::Handle<v8::Value> GetPointX(v8::Local<v8::String> property,
                          const v8::AccessorInfo &info) {
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int value = static_cast<Point*>(ptr)->x_;
	return v8::Number::New(value);
}

void SetPointX(v8::Local<v8::String> property, v8::Local<v8::Value> value,
             const v8::AccessorInfo& info) {
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Point*>(ptr)->x_ = value->Int32Value();
}

v8::Handle<v8::Value> GetPointY(v8::Local<v8::String> property,
                          const v8::AccessorInfo &info) {
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int value = static_cast<Point*>(ptr)->y_;
	return v8::Number::New(value);
}

void SetPointY(v8::Local<v8::String> property, v8::Local<v8::Value> value,
             const v8::AccessorInfo& info) {
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Point*>(ptr)->y_ = value->Int32Value();
}

// -----------------------------------------------------------------------------------------
// --- Person --------------------------------------------------------------------------------------

class Person {
public:
Person(std::string name, int age) : name_(name), age_(age) { }
std::string name_;
int age_;
};

v8::Handle<v8::Value> GetPersonAge(v8::Local<v8::String> property,
                          const v8::AccessorInfo &info) {
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	int value = static_cast<Person*>(ptr)->age_;
	return v8::Number::New(value);
}

void SetPersonAge(v8::Local<v8::String> property, v8::Local<v8::Value> value,
             const v8::AccessorInfo& info) {
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Person*>(ptr)->age_ = value->Int32Value();
}

v8::Handle<v8::Value> GetPersonName(v8::Local<v8::String> property,
                          const v8::AccessorInfo &info) {
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	std::string value = static_cast<Person*>(ptr)->name_;
	return v8::String::New(value.c_str());
}

void SetPersonName(v8::Local<v8::String> property, v8::Local<v8::Value> value,
             const v8::AccessorInfo& info) {
	v8::Local<v8::Object> self = info.Holder();
	v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
	void* ptr = wrap->Value();
	static_cast<Person*>(ptr)->name_ = "johan";
}

// -----------------------------------------------------------------------------------------

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

class Line {
    std::string data;
public:
    friend std::istream &operator>>(std::istream &is, Line &l) {
		std::getline(is, l.data);
		//replaceAll(l.data, "\\", ";");
		//replaceAll(l.data, ";;", ";");
        return is;
    }
    operator std::string() const { return data; }    
};

struct LineReader: std::ctype<char> {
    LineReader(): std::ctype<char>(get_table()) {}
    static std::ctype_base::mask const* get_table() {
        static std::vector<std::ctype_base::mask> 
            rc(table_size, std::ctype_base::mask());
        rc['\n'] = std::ctype_base::space;
        return &rc[0];
    }
};  

const char* ToCString(const v8::String::Utf8Value& value) 
{
    return *value ? *value : "<string conversion failed>";
}


void Core::execute(const std::string& str) {

	v8::Handle<v8::Object> global = context->Global();
    v8::HandleScope handle_scope;
    v8::TryCatch try_catch;

	bool print_result = true;

    v8::Handle<v8::Script> script = v8::Script::Compile(v8::String::New(str.c_str()), v8::String::New("quantjs"));

    if (!script.IsEmpty()) {

        v8::Handle<v8::Value> result = script->Run();

        v8::String::Utf8Value exception(try_catch.Exception());
        const char* exception_string = ToCString(exception);
        v8::Handle<v8::Message> message = try_catch.Message();

        if (!message.IsEmpty()) {
            fprintf(stderr, "\e[0;31mOut[1]: %s\n\e[0m", exception_string);
        }

        if (!result.IsEmpty() && print_result && !result->IsUndefined()) {
            
            if (result->IsArray()) {

                v8::Local<v8::Array> array = v8::Local<v8::Array>::Cast(result->ToObject());
                for (unsigned int i = 0; i < array->Length(); ++i) {
                    v8::Local<v8::Value> v8_value = array->Get(i);
                    
                    if (v8_value->IsArray()) {
                        v8::Local<v8::Array> arrayInside = v8::Local<v8::Array>::Cast(v8_value->ToObject());
                        printf("\t[%u]\t", i);
                        for (unsigned int j = 0; j < arrayInside->Length(); ++j) {
                            v8::Local<v8::Value> v8_valueInside = arrayInside->Get(j);
                            v8::String::Utf8Value valueInside(v8_valueInside->ToDetailString());
                            printf("\t%s ", *valueInside);
                        }
                        printf("\n");
                    }
                    else {
                        v8::String::Utf8Value value(v8_value->ToDetailString());
                        printf("\t[%u]\t%s\n", i, *value);
                    }
                }

            } else if (result->IsNumber()) {
                v8::Local<v8::Number> number = result->ToNumber();

                printf("\t%4.4f\n", number->NumberValue());

            } else if (result->IsObject()) {
                v8::Local<v8::Object> object = result->ToObject();
                v8::Local<v8::Array> properties = object->GetOwnPropertyNames();
                for (unsigned int i = 0; i < properties->Length(); ++i) {
                    v8::Local<v8::Value> v8_value = object->Get(properties->Get(i));
                    v8::String::Utf8Value value(v8_value->ToString());
                    v8::String::Utf8Value key(properties->Get(i)->ToString());
                    printf("\t[%s]\t%s\n", *key, *value);
                }
            } else {
                v8::String::Utf8Value value(result->ToDetailString());
                printf("\t%s\n", *value);
            }
        } 
    }
}

std::string Core::promptString() {
	std::stringstream ss;

	ss << "qjs[";
	ss << ++promptLine;
	ss << "]>: ";

	return ss.str();	
}

void Core::RunShell() {

	using namespace v8;

    // Init and start V8
    HandleScope handle_scope;

    v8::Handle<v8::ObjectTemplate> global = ObjectTemplate::New();
    context = Context::New(NULL, global);
    Context::Scope context_scope(context);

    Handle<FunctionTemplate> engine_templ = FunctionTemplate::New();
    engine_templ->SetClassName(String::New("QuantJS API"));

    Handle<ObjectTemplate> engine_proto = engine_templ->PrototypeTemplate();
   
    Handle<ObjectTemplate> engine_inst = engine_templ->InstanceTemplate();
    engine_inst->SetInternalFieldCount(1);


	// Point
	Local<ObjectTemplate> point_templ = ObjectTemplate::New();
	point_templ->SetInternalFieldCount(1);
	point_templ->SetAccessor(String::New("x"), GetPointX, SetPointX);
	point_templ->SetAccessor(String::New("y"), GetPointY, SetPointY);
	Point* p = new Point(10,11);
  	Local<Object> obj = point_templ->NewInstance();
	obj->SetInternalField(0, External::New(p));
    context->Global()->Set(String::New("p"), obj);


	// Person
	Local<ObjectTemplate> person_templ = ObjectTemplate::New();
	person_templ->SetInternalFieldCount(1);
	person_templ->SetAccessor(String::New("age"), GetPersonAge, SetPersonAge);
	person_templ->SetAccessor(String::New("name"), GetPersonName, SetPersonName);
	Person* pp = new Person("johan", 29);
  	Local<Object> pobj = person_templ->NewInstance();
	pobj->SetInternalField(0, External::New(pp));
    context->Global()->Set(String::New("person"), pobj);


	// Run REPL
    std::cin.imbue(std::locale(std::locale(), new LineReader()));

	std::cout << promptString();

    std::for_each(std::istream_iterator<Line>(std::cin), 
        std::istream_iterator<Line>(), 
        [=](const std::string& s) { 
			// TODO: wrap this pointer
			this->execute(s);
			std::cout << std::endl << promptString();
		});
}

void Core::run() {

    // Check options
    if (!options.validate()) return;
        std::cout << options << std::endl;
	
	RunShell();	
}
