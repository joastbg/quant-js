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

// TODO: move
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

// TODO: move
class Core {
public:
    Core(Options options) : options(options) {}
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

void Core::run() {

    // Check options
    if (!options.validate()) return;
        std::cout << options << std::endl;

    // Init and start V8
    HandleScope handle_scope;

    v8::Handle<v8::ObjectTemplate> global = ObjectTemplate::New();
    context = Context::New(NULL, global);
    Context::Scope context_scope(context);

       Handle<FunctionTemplate> engine_templ = FunctionTemplate::New();
    engine_templ->SetClassName(String::New("QuantJS API"));

    Handle<ObjectTemplate> engine_proto = engine_templ->PrototypeTemplate();

    // use of proxy
    v8_proxy proxy;

    proxy.add(engine_proto, "johan", fjohan);
    proxy.add(engine_proto, "nisse",  fnisse);
    proxy.list_all();            

    ///

    engine_proto->Set("version", FunctionTemplate::New(Version));
    
    Handle<ObjectTemplate> engine_inst = engine_templ->InstanceTemplate();
    engine_inst->SetInternalFieldCount(1);

    QuantEngine* engine = new QuantEngine();
    Handle<Function> engine_ctor = engine_templ->GetFunction();
    Local<Object> engine_obj = engine_ctor->NewInstance();
    engine_obj->SetInternalField(0, External::New(engine));

    context->Global()->Set(String::New("quantjs"), engine_obj);

    // Libraries

    std::vector<std::string> files;
    files.push_back("quantjs.js");
    files.push_back("underscore-min.js");
    files.push_back("numeric-1.2.6.min.js");

    for (int i=0;i<files.size();i++) {
        v8::Handle<v8::String> source = ReadFile(files[i].c_str());
        Handle<Script> script = Script::Compile(source);
        Handle<Value> result = script->Run();
    }


    // REPL

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

    Core Core(options);
    Core.run();
    
    return 0;
}
