#include <v8.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <functional>
#include <stdlib.h>
#include <pthread.h>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/thread/thread.hpp>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include "options.h"

#define QJS_VERSION "0.0.4"

using namespace v8;
using namespace boost::gregorian;
using namespace boost::posix_time;

Persistent<Context> context;

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

void usage() {
	std::cout << "usage: quantjs [--infiles filename(s)...] [--help]" << std::endl;
}

void banner() {
	std::cout << "quantjs v" << QJS_VERSION << std::endl;
	std::cout << "Copyright 2013-2014 Johan Astborg" << std::endl;
}


struct v8_proxy {

	void add(v8::Handle<v8::ObjectTemplate>& ot, const std::string& name, 
		std::function<void()> fun) {

		fps.push_back(name);

		// Not the most beautiful pattern (redesign)

		ot->Set(v8::String::New(name.c_str()), v8::FunctionTemplate::New(
			[](const v8::Arguments& args)->v8::Handle<v8::Value>
			{
				v8::Handle<v8::External> data = v8::Handle<v8::External>::Cast(args.Data());
				std::function<void()>* out = static_cast<std::function<void()>*>(data->Value());

				if ( args.Length() != 1 ) {
					return v8::ThrowException(v8::String::New("Wrong nr of args."));
				}
			
				v8::String::AsciiValue ascii(args[0]);
				(*out)();
				std::cout << *ascii << "\n";
			
				return v8::Undefined();
			}, v8::External::New(&fun)
		));

	}

	void list_all() {

		for (auto s : fps) {
			std::cout << s << std::endl;
		}
	}

	std::vector<std::string> fps;
};

class ARCore {
public:
	ARCore(Options options) : options(options) {}
	void run();
private:
	const Options& options;
};

void fjohan() {
	std::cout << "From function fjohan\n";
}

void fnisse() {
	std::cout << "From function fnisse\n";
}

// Reads a file into a v8 string.
v8::Handle<v8::String> ReadFile(const char* name) {

	std::ifstream infile (name, std::ios_base::in);

	if (!infile.good()) return v8::Handle<v8::String>();

	std::stringstream sstream;
	while (infile.good() && infile.peek() != -1) {
		sstream.put(infile.get());
	}
	infile.close();

	v8::Handle<v8::String> result = v8::String::New(sstream.str().c_str());

	return result;
}

class QuantEngine {
    public:
        QuantEngine() : version(QJS_VERSION) {};

        std::string& Version() {
            return version;
        }      

    private:
        std::string version;
};

Handle<Value> Version(const Arguments& args)
{
    Local<Object> self = args.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();

    return v8::String::New(static_cast<QuantEngine*>(ptr)->Version().c_str());
}

void ARCore::run() {
	std::cout << std::endl << "-- ready" << std::endl << std::endl;

	// Check options
	if (!options.validate()) return;
		std::cout << options << std::endl;

	// JS part
	HandleScope handle_scope;

	v8::Handle<v8::ObjectTemplate> global = ObjectTemplate::New();
    context = Context::New(NULL, global);
    Context::Scope context_scope(context);

	/////////////////7

	
	//Handle<FunctionTemplate> console_templ = FunctionTemplate::New();
    //console_templ->SetClassName(String::New("Console"));
    //Handle<ObjectTemplate> console_proto = console_templ->PrototypeTemplate();
    //console_proto->Set("log", FunctionTemplate::New(ConsoleMethod_Log));
    //Handle<ObjectTemplate> console_inst = console_templ->InstanceTemplate();
    //console_inst->SetInternalFieldCount(1);

    //Console* console = new Console();
    //Handle<Function> console_ctor = console_templ->GetFunction();
    //Local<Object> console_obj = console_ctor->NewInstance();
    //console_obj->SetInternalField(0, External::New(console));
    //context->Global()->Set(String::New("console"), console_obj);

   	Handle<FunctionTemplate> engine_templ = FunctionTemplate::New();
    engine_templ->SetClassName(String::New("QuantJS API"));

    Handle<ObjectTemplate> engine_proto = engine_templ->PrototypeTemplate();
	
    engine_proto->Set("version", FunctionTemplate::New(Version));
    //raptor_proto->Set("subscribe", FunctionTemplate::New(Raptor_Subscribe));

    //raptor_proto->Set("Print", FunctionTemplate::New(Print));
    //raptor_proto->Set("eqtest", FunctionTemplate::New(Equity_Option_01));
	/*
    //// Math functions
    raptor_proto->Set("log", FunctionTemplate::New(log));
    raptor_proto->Set("log10", FunctionTemplate::New(log10));
    raptor_proto->Set("sin", FunctionTemplate::New(log10));
    raptor_proto->Set("cos", FunctionTemplate::New(log10));
    raptor_proto->Set("tan", FunctionTemplate::New(log10));
	raptor_proto->Set("callme", FunctionTemplate::New(callme));
	raptor_proto->Set("load", FunctionTemplate::New(load));

    raptor_proto->Set("sleep", FunctionTemplate::New(sleep));
    raptor_proto->Set("timems", FunctionTemplate::New(timems));

    raptor_proto->Set("rnd", FunctionTemplate::New(randfloat));

    // TODO: Ask before exit
    raptor_proto->Set("exit", FunctionTemplate::New(exit));


	raptor_proto->Set("require", FunctionTemplate::New(require));
	*/

	// New design...
	
	//v8_proxy proxy;
	
	//proxy.add(raptor_proto, "johan", fjohan);
	//proxy.add(raptor_proto, "nisse",  fnisse);
	//proxy.list_all();


	//raptor_proto->Set("apan", FunctionTemplate::New(glambda));

    Handle<ObjectTemplate> engine_inst = engine_templ->InstanceTemplate();
    engine_inst->SetInternalFieldCount(1);

    QuantEngine* engine = new QuantEngine();
    Handle<Function> engine_ctor = engine_templ->GetFunction();
    Local<Object> engine_obj = engine_ctor->NewInstance();
    engine_obj->SetInternalField(0, External::New(engine));

	context->Global()->Set(String::New("quantjs"), engine_obj);

	///////////

	std::vector<std::string> files;
	files.push_back("quantjs.js");

	// http://underscorejs.org/
	files.push_back("underscore-min.js");
	
	// http://numericjs.com/documentation.html
	files.push_back("numeric-1.2.6.min.js");

	for (int i=0;i<files.size();i++) {
		v8::Handle<v8::String> source = ReadFile(files[i].c_str());
		Handle<Script> script = Script::Compile(source);
		Handle<Value> result = script->Run();
	}
		
    //Handle<Value> befunc = context->Global()->Get(String::New("init"));
    //Handle<Function> func = Handle<Function>::Cast(befunc);
    //Handle<Value> args[0];

    //Handle<Value> js_result = func->Call(context->Global(), 0, args);	

	bool print_result = true;

	while (true) {
        char buffer[256];
        printf("qjs> ");
        char* str = fgets(buffer, 256, stdin);
        if (str == NULL) break;
        v8::HandleScope handle_scope;
		v8::TryCatch try_catch;

        Handle<Script> script = Script::Compile(String::New(str), String::New("quantjs"));

		if (!script.IsEmpty()) {

	        v8::Handle<v8::Value> result = script->Run();

			v8::String::Utf8Value exception(try_catch.Exception());
	  		const char* exception_string = ToCString(exception);
			v8::Handle<v8::Message> message = try_catch.Message();		
			if (!message.IsEmpty()) {
				fprintf(stderr, "%s\n", exception_string);
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
								printf("%s ", *valueInside);
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
}

int main(int argc, char* argv[]) {

	Options options;
	if (argc == 1) usage();	
	for (int i = 1; i < argc; ++i) {
		if (!strncmp(argv[i], "--infiles", 8)) options.infile = argv[++i];
		if (!strncmp(argv[i], "--help", 6) || !strncmp(argv[i], "-h", 2)) usage();
	}

	banner();

	// Init quantjs
	ARCore arcore(options);
	arcore.run();
	
    return 0;
}
