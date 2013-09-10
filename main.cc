#include <v8.h> 
#include <iostream>
#include <fstream>

#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <pthread.h>
//#include <ev.h>

// the only header you need to use QuantLib
#include <ql/quantlib.hpp>
//using namespace QuantLib;

using namespace v8;

using namespace boost::gregorian;
using namespace boost::posix_time;

int x = 6;

//////

static pthread_t texample_thread;
//static ev_async eio_texample_notifier;

static void* TheThread(void *) {
    int i = 0;
    while(true) {
        // fire event every 5 seconds
        sleep(5);
        //pthread_mutex_lock(&queue_mutex);
        //cb_msg_queue.push(i);
        //pthread_mutex_unlock(&queue_mutex);
        i++;
        // wake up callback
        //ev_async_send(EV_DEFAULT_UC_ &eio_texample_notifier);
        std::cout << "HOHO" << std::endl;
    }
    return NULL;
}

//////

v8::Handle<v8::Value> Plus(const v8::Arguments& args)
{ 
    unsigned int A = args[0]->Uint32Value();
    unsigned int B = args[1]->Uint32Value();
    
    return v8::Number::New(A +  B);
}

v8::Handle<v8::Value> buy(const v8::Arguments& args)
{
    v8::String::AsciiValue str(args[0]->ToString());
    std::cout << *str << std::endl;
    return v8::Undefined();
}

v8::Handle<v8::Value> Print(const v8::Arguments& args) {

    if (args[0]->IsObject()) {
        Handle<Object> object = Handle<Object>::Cast(args[0]);

        // optionType, underlyingPrice, strikePrice, dividendYield, riskFreeRate, volatility
        Handle<Value> optionType = object->Get(String::New("optionType"));
        Handle<Value> underlyingPrice = object->Get(String::New("underlyingPrice"));
        Handle<Value> strikePrice = object->Get(String::New("strikePrice"));
        Handle<Value> dividendYield = object->Get(String::New("dividendYield"));
        Handle<Value> riskFreeRate = object->Get(String::New("riskFreeRate"));
        Handle<Value> volatility = object->Get(String::New("volatility"));

        v8::String::AsciiValue optionTypeStr(optionType->ToString());
        std::cout << *optionTypeStr << std::endl;
        std::cout << "";

        v8::String::AsciiValue underlyingPriceStr(underlyingPrice->ToString());
        std::cout << *underlyingPriceStr << std::endl;
        std::cout << "";

        v8::String::AsciiValue strikePriceStr(strikePrice->ToString());
        std::cout << *strikePriceStr << std::endl;
        std::cout << "";

        v8::String::AsciiValue dividendYieldStr(dividendYield->ToString());
        std::cout << *dividendYieldStr << std::endl;
        std::cout << "";

        v8::String::AsciiValue riskFreeRateStr(riskFreeRate->ToString());
        std::cout << *riskFreeRateStr << std::endl;
        std::cout << "";

        v8::String::AsciiValue volatilityStr(volatility->ToString());
        std::cout << *volatilityStr << std::endl;
        std::cout << "";
    }

    for (int i = 0; i < args.Length(); i++) {
        v8::String::AsciiValue str(args[i]->ToString());
        std::cout << *str << std::endl;
        std::cout << "";
        //date d();
        //date_facet* facet(new date_facet("%Y-%m-%d T%H%M%S%F%q"));
        //std::cout.imbue(std::locale(std::cout.getloc(), facet));
        //std::cout << d << std::endl;
        
    
        //std::string s("2001-10-9"); //2001-October-09
        //date d(from_simple_string(s));
    }
    return v8::Undefined();
}

Handle<Value> XGetter(Local<String> property, const AccessorInfo& info) {
    return Integer::New(x);
}
    
void XSetter(Local<String> property, Local<Value> value, const AccessorInfo& info) {
    x = value->Int32Value();
}

class Point 
{ 
public: 
    //constructor
    Point(int x, int y):x_(x),y_(y){}

    //internal class functions
    //just increment x_
    void Function_A(){++x_;    }

    //increment x_ by the amount
    void Function_B(int vlr){x_+=vlr;}

    //variables
    int x_;
    int y_;
};

// Taken from the embedder's guide
Handle<Value> GetPointX(Local<String> property, const AccessorInfo& info)
{
    //this only shows information on what object is being used... just for fun
    {
        String::AsciiValue holder(info.Holder()->ToString()), self(info.This()->ToString());
        printf("getter: holder(%s), self(%s)\n", *holder, *self);
    }
    //get object holder
    Local<Object> self = info.Holder();
    //get the holder's external object
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    //get the pointer for Point
    void* ptr = wrap->Value();
    //get member variable value
    int value = static_cast<Point*>(ptr)->x_;
    //return the value
    return Integer::New(value);
}

// Taken from the embedder's guide
void SetPointX(Local<String> property, Local<Value> value, const AccessorInfo& info)
{
    Local<Object> self = info.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Point*>(ptr)->x_ = value->Int32Value();
}

// Returns the native Point's x value
Handle<Value> PointMethod_A(const Arguments& args)
{
    Local<Object> self = args.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    //use Function_A from Point pointer
    static_cast<Point*>(ptr)->Function_A();
    return Integer::New(static_cast<Point*>(ptr)->x_);
}
Handle<Value> PointMethod_B(const Arguments& args)
{
    Local<Object> self = args.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    //use Function_B from Point pointer
    static_cast<Point*>(ptr)->Function_B(args[0]->Uint32Value());
    return Integer::New(static_cast<Point*>(ptr)->x_);
}

class Console {
    public:
        void Log(v8::Local<v8::String> str) {
            v8::String::AsciiValue astr(str);
            std::cout << to_iso_extended_string(microsec_clock::local_time()) << " INFO\t- " << *astr << std::endl;
        }
};

Handle<Value> ConsoleMethod_Log(const Arguments& args)
{
    Local<Object> self = args.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    //use Function_B from Point pointer
    static_cast<Console*>(ptr)->Log(args[0]->ToString());
//  return Integer::New(static_cast<Point*>(ptr)->x_);
    return v8::Undefined();
}

// Callbacks
//

/*
static void Callback(EV_P_ ev_async *watcher, int revents)
{
    //std::cout << "MONKEY FARM" << std::endl;
}*/

// -------- RAPTOR
//

class RaptorAPI {
    public:
        RaptorAPI() : version("Raptor 0.02") {};
    
        std::string& Version() {
            return version;
        }

        void Subscribe() {

        }

    private:
        std::string version;
};

Handle<Value> Raptor_Version(const Arguments& args)
{
    Local<Object> self = args.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
        
    return v8::String::New(static_cast<RaptorAPI*>(ptr)->Version().c_str());
}

Handle<Value> Equity_Option_01(const Arguments& args)
{
    // TODO: get parameters from args
    double underlyingDouble = 0.0;
    double strikeDouble = 0.0;
    double dividendYieldDouble = 0.0;
    double riskFreeRateDouble = 0.0;
    double volatilityDouble = 0.0;

    if (args[0]->IsObject()) {
        Handle<Object> object = Handle<Object>::Cast(args[0]);

        // optionType, underlyingPrice, strikePrice, dividendYield, riskFreeRate, volatility
        Handle<Value> optionType = object->Get(String::New("optionType"));
        Handle<Value> underlyingPrice = object->Get(String::New("underlyingPrice"));
        Handle<Value> strikePrice = object->Get(String::New("strikePrice"));
        Handle<Value> dividendYield = object->Get(String::New("dividendYield"));
        Handle<Value> riskFreeRate = object->Get(String::New("riskFreeRate"));
        Handle<Value> volatility = object->Get(String::New("volatility"));

        v8::String::AsciiValue optionTypeStr(optionType->ToString());

        if (underlyingPrice->IsNumber()) {
            Handle<Number> underlyingPriceNum = Handle<Number>::Cast(underlyingPrice);
            //v8::Number underlyingPriceNumber(underlyingPriceNum->value);
            //std::cout << underlyingPriceNum << std::endl;
            
            v8::String::AsciiValue testValue(underlyingPriceNum->ToString());
            //v8::Number numberValue(underlyingPriceNum);
            //std::cout << *testValue << std::endl;
            underlyingDouble = atof(*testValue);
            std::cout << underlyingDouble << std::endl;
        }

        if (strikePrice->IsNumber()) {
            Handle<Number> strikePriceNum = Handle<Number>::Cast(strikePrice);
            //v8::Number underlyingPriceNumber(underlyingPriceNum->value);
            //std::cout << underlyingPriceNum << std::endl;
            
            v8::String::AsciiValue testValue(strikePriceNum->ToString());
            //v8::Number numberValue(underlyingPriceNum);
            //std::cout << *testValue << std::endl;
            strikeDouble = atof(*testValue);
            std::cout << strikeDouble << std::endl;
        }

        if (dividendYield->IsNumber()) {
            Handle<Number> dividendYieldNum = Handle<Number>::Cast(dividendYield);
            //v8::Number underlyingPriceNumber(underlyingPriceNum->value);
            //std::cout << underlyingPriceNum << std::endl;
            
            v8::String::AsciiValue testValue(dividendYieldNum->ToString());
            //v8::Number numberValue(underlyingPriceNum);
            //std::cout << *testValue << std::endl;
            dividendYieldDouble = atof(*testValue);
            std::cout << dividendYieldDouble << std::endl;
        }

        if (riskFreeRate->IsNumber()) {
            Handle<Number> riskFreeRateNum = Handle<Number>::Cast(riskFreeRate);
            //v8::Number underlyingPriceNumber(underlyingPriceNum->value);
            //std::cout << underlyingPriceNum << std::endl;
            
            v8::String::AsciiValue testValue(riskFreeRateNum->ToString());
            //v8::Number numberValue(underlyingPriceNum);
            //std::cout << *testValue << std::endl;
            riskFreeRateDouble = atof(*testValue);
            std::cout << riskFreeRateDouble << std::endl;
        }

        if (volatility->IsNumber()) {
            Handle<Number> volatilityNum = Handle<Number>::Cast(volatility);
            //v8::Number underlyingPriceNumber(underlyingPriceNum->value);
            //std::cout << underlyingPriceNum << std::endl;
            
            v8::String::AsciiValue testValue(volatilityNum->ToString());
            //v8::Number numberValue(underlyingPriceNum);
            //std::cout << *testValue << std::endl;
            volatilityDouble = atof(*testValue);
            std::cout << volatilityDouble << std::endl;
        }
    }

    // set up dates
    QuantLib::Calendar calendar = QuantLib::TARGET();
    QuantLib::Date todaysDate(15, QuantLib::May, 1998);
    QuantLib::Date settlementDate(17, QuantLib::May, 1998);
    QuantLib::Settings::instance().evaluationDate() = todaysDate;

    // our options
    QuantLib::Option::Type type(QuantLib::Option::Put);
    QuantLib::Real underlying = underlyingDouble;
    QuantLib::Real strike = strikeDouble;
    QuantLib::Spread dividendYield = dividendYieldDouble;
    QuantLib::Rate riskFreeRate = riskFreeRateDouble;
    QuantLib::Volatility volatility = volatilityDouble;
    QuantLib::Date maturity(17, QuantLib::May, 1999);
    QuantLib::DayCounter dayCounter = QuantLib::Actual365Fixed();

    // Black-Scholes for European
    boost::shared_ptr<QuantLib::Exercise> europeanExercise(new QuantLib::EuropeanExercise(maturity));

    boost::shared_ptr<QuantLib::StrikedTypePayoff> payoff(new QuantLib::PlainVanillaPayoff(type, strike));

    QuantLib::VanillaOption europeanOption(payoff, europeanExercise);

    //boost::shared_ptr<QuantLib::BlackScholesMertonProcess> bsmProcess(new QuantLib::BlackScholesMertonProcess(underlyingH, flatDividendTS, flatTermStructure, flatVolTS));
    //QuantLib::

    QuantLib::Handle<QuantLib::Quote> underlyingH(boost::shared_ptr<QuantLib::Quote>(new QuantLib::SimpleQuote(underlying)));

    // bootstrap the yield/dividend/vol curves
    QuantLib::Handle<QuantLib::YieldTermStructure> flatTermStructure(boost::shared_ptr<QuantLib::YieldTermStructure>(new QuantLib::FlatForward(settlementDate, riskFreeRate, dayCounter)));
    QuantLib::Handle<QuantLib::YieldTermStructure> flatDividendTS(boost::shared_ptr<QuantLib::YieldTermStructure>(new QuantLib::FlatForward(settlementDate, dividendYield, dayCounter)));
    QuantLib::Handle<QuantLib::BlackVolTermStructure> flatVolTS(boost::shared_ptr<QuantLib::BlackVolTermStructure>(new QuantLib::BlackConstantVol(settlementDate, calendar, volatility, dayCounter)));
    
    boost::shared_ptr<QuantLib::BlackScholesMertonProcess> bsmProcess(new QuantLib::BlackScholesMertonProcess(underlyingH, flatDividendTS,flatTermStructure, flatVolTS));

    //europeanOption.setPricingEngine(boost::shared_ptr<PricingEngine>(new AnalyticEuropeanEngine(bsmProcess)));
    europeanOption.setPricingEngine(boost::shared_ptr<QuantLib::PricingEngine>(new QuantLib::AnalyticEuropeanEngine(bsmProcess)));

  

    return Number::New(europeanOption.NPV());
}

Handle<Value> Raptor_Subscribe(const Arguments& args) {

    Local<Object> self = args.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    
    static_cast<RaptorAPI*>(ptr)->Subscribe();  


    //Handle<Value> befunc = context->Global()->Get(String::New("init"));
    Handle<Function> func = Handle<Function>::Cast(args[2]);
    Handle<Value> argv[0];
    Handle<Value> js_result = func->Call(self, 0, argv);

    
    return v8::Undefined();
}

// ------

//ev_io stdin_watcher;
//ev_timer timeout_watcher;

// another callback, this time for a time-out
/*
static void
timeout_cb (EV_P_ ev_timer *w, int revents) {
    puts ("timeout");
}*/

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

int main(int argc, char* argv[]) {
    // Create a stack-allocated handle scope. 
    HandleScope handle_scope;
    // Create a new context. 
    //Handle<Context> context = Context::New();
    // Enter the created context for compiling and 
    // running the hello world script.
 
    std::cout << "Loading file: " << argv[1] << std::endl;
  
    std::ifstream infile (argv[1], std::ios_base::in);
    std::stringstream sstream;
    while (infile.good() && infile.peek() != -1) {
        sstream.put(infile.get());
    }
    infile.close();

    // Add custom function  
    v8::Handle<v8::ObjectTemplate> global = ObjectTemplate::New();
    global->Set(v8::String::New("plus"), FunctionTemplate::New(Plus));
    global->Set(v8::String::New("buy"), FunctionTemplate::New(buy));
    global->Set(v8::String::New("Print"), FunctionTemplate::New(Print));

    global->SetAccessor(String::New("x"), XGetter, XSetter);

    Persistent<Context> context = Context::New(NULL, global);
    Context::Scope context_scope(context);


    // Map Point class
    /*
    Handle<FunctionTemplate> point_templ = FunctionTemplate::New();
    point_templ->SetClassName(String::New("Point"));
    Handle<ObjectTemplate> point_proto = point_templ->PrototypeTemplate();
    point_proto->Set("method_a", FunctionTemplate::New(PointMethod_A));
    point_proto->Set("method_b", FunctionTemplate::New(PointMethod_B));
    Handle<ObjectTemplate> point_inst = point_templ->InstanceTemplate();
    point_inst->SetInternalFieldCount(1);
    point_inst->SetAccessor(String::New("x"), GetPointX, SetPointX);

    Point* p = new Point(0, 0);

    Handle<Function> point_ctor = point_templ->GetFunction();
    Local<Object> obj = point_ctor->NewInstance();
    obj->SetInternalField(0, External::New(p));

    context->Global()->Set(String::New("point"), obj);
    */

    // Map Console class
    Handle<FunctionTemplate> console_templ = FunctionTemplate::New();
    console_templ->SetClassName(String::New("Console"));
    Handle<ObjectTemplate> console_proto = console_templ->PrototypeTemplate();
    console_proto->Set("log", FunctionTemplate::New(ConsoleMethod_Log));
    Handle<ObjectTemplate> console_inst = console_templ->InstanceTemplate();
    console_inst->SetInternalFieldCount(1);
    
    Console* console = new Console();
    Handle<Function> console_ctor = console_templ->GetFunction();
    Local<Object> console_obj = console_ctor->NewInstance();
    console_obj->SetInternalField(0, External::New(console));
    context->Global()->Set(String::New("console"), console_obj);

    // Create a string containing the JavaScript source code. 
    //Handle<String> source = String::New("var a=1;Print('a: ' + a);Print('x: ' + x);plus(1,2);buy('das');function foo() {  Print(Math.abs(-2)); };monkey = function(data, err) { Print(data);};foo();");
    //
    
    //// Raptor
    
    Handle<FunctionTemplate> raptor_templ = FunctionTemplate::New();
    raptor_templ->SetClassName(String::New("RaptorAPI"));
    
    Handle<ObjectTemplate> raptor_proto = raptor_templ->PrototypeTemplate();
    
    raptor_proto->Set("version", FunctionTemplate::New(Raptor_Version));
    raptor_proto->Set("subscribe", FunctionTemplate::New(Raptor_Subscribe));

    raptor_proto->Set("Print", FunctionTemplate::New(Print));
    raptor_proto->Set("eqtest", FunctionTemplate::New(Equity_Option_01));

    Handle<ObjectTemplate> raptor_inst = raptor_templ->InstanceTemplate();
    raptor_inst->SetInternalFieldCount(1);

    RaptorAPI* raptor = new RaptorAPI();
    Handle<Function> raptor_ctor = raptor_templ->GetFunction();
    Local<Object> raptor_obj = raptor_ctor->NewInstance();
    raptor_obj->SetInternalField(0, External::New(raptor));
    context->Global()->Set(String::New("raptor"), raptor_obj);

    //////
    
    Handle<String> source = String::New(sstream.str().c_str());
    
    // Compile the source code. 
    Handle<Script> script = Script::Compile(source);

    // Run the script to get the result. 
    Handle<Value> result = script->Run();

    //Handle<Value> value = context->Get(String::New("foo"));

    //Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(value);
    //Handle<Value> js_result = func->Call(global, 0, null);

    Handle<Value> befunc = context->Global()->Get(String::New("init"));
    Handle<Function> func = Handle<Function>::Cast(befunc);
    Handle<Value> args[0];
    //args[0] = String::New("monkey is nice");
    //args[1] = String::New("hoho");
    Handle<Value> js_result = func->Call(context->Global(), 0, args);

    v8::Local<v8::String> name(v8::String::New("raptor"));

    while (true) {
        char buffer[256];
        printf("> ");
        char* str = fgets(buffer, 256, stdin);
        if (str == NULL) break;
        v8::HandleScope handle_scope;
        //ExecuteString(v8::String::New(str), name, true, true);

        Handle<Script> script = Script::Compile(String::New(str), name);
        v8::Handle<v8::Value> result = script->Run();
        //v8::Handle<v8::Script> script = v8::Script::Compile(source, name);
        String::Utf8Value str2(result);
        const char* cstr = ToCString(str2);
        printf("%s\n", cstr);
    }

    /*
    struct ev_loop *loop = EV_DEFAULT;
    ev_timer_init (&timeout_watcher, timeout_cb, 0., 5.);
    ev_timer_again (loop, &timeout_watcher);
*/
    //pthread_create(&texample_thread, NULL, TheThread, 0);


    //pthread_create(&texample_thread, NULL, TheThread, 0);

   // ev_run (loop, 0);

    //ev_async_init(&eio_texample_notifier, Callback);
    //ev_async_start(EV_DEFAULT_UC_ &eio_texample_notifier);
    //ev_unref(EV_DEFAULT_UC);


    pthread_join(texample_thread, NULL);

    // Convert the result to an ASCII string and print it. 
    //String::AsciiValue ascii(result);
    //printf("%s\n", *ascii);
    return 0;
}
