#include <v8.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <vector>
#include <stdio.h>
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

typedef boost::mt19937                                      T_base_prng;
typedef boost::normal_distribution<>                        T_norm_dist;
typedef boost::variate_generator<T_base_prng&, T_norm_dist> T_norm_varg;

#include <ql/quantlib.hpp>

#include "options.h"

using namespace v8;
using namespace boost::gregorian;
using namespace boost::posix_time;

Persistent<Context> context;

v8::Handle<v8::Value> Print(const v8::Arguments& args) {

    if (args[0]->IsObject()) {
        Handle<Object> object = Handle<Object>::Cast(args[0]);

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
    }
    return v8::Undefined();
}

class Console {
    public:
        v8::Handle<v8::Value> Log(v8::Local<v8::String> str) {
            v8::String::AsciiValue astr(str);
            std::cout << to_iso_extended_string(microsec_clock::local_time()) << " INFO\t- " << *astr << std::endl;

			return v8::Undefined();
        }
};

v8::Handle<v8::Value> exit(const v8::Arguments& args)
{	
    // Hack to exit app
    exit(0);
}

boost::random::mt19937 gen;
boost::random::uniform_real_distribution<> dist(1, 3.14159);

v8::Handle<v8::Value> callme(const v8::Arguments& args)
{
	Handle<v8::Object> global = context->Global();

	if (args[2]->IsFunction()) {
		Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(args[2]->ToObject());
		Handle<Value> argsf[2];
	    
	    //argsf[1] = v8::Number::New(0.02);

		double acc=0;	

		Handle<Number> aNum = Handle<Number>::Cast(args[0]);
		Handle<Number> bNum = Handle<Number>::Cast(args[1]);

		double a = aNum->NumberValue();
		double b = bNum->NumberValue();

		boost::random::uniform_real_distribution<> dist2(a, b);

		for (int i=0;i<50000;i++) {
		
			argsf[0] = v8::Number::New(dist2(gen));
			Handle<Value> js_result = func->Call(global, 1, argsf);

			//Handle<Number> strikePriceNum = Handle<Number>::Cast(strikePrice);
            //v8::String::AsciiValue testValue(strikePriceNum->ToString());
            //strikeDouble = atof(*testValue);

			acc += js_result->NumberValue();

			v8::String::Utf8Value value(js_result->ToDetailString());
			//printf("\t%s\n", *value);
		}
		std::cout << "I: " << (b-a)*acc/50000.0 << std::endl;
	}

	return v8::Undefined();
}

v8::Handle<v8::Value> log10(const v8::Arguments& args)
{
    // Returns the common (base-10) logarithm of x

    v8::String::AsciiValue argStr(args[0]->ToString());
    //std::cout << *argStr << std::endl;
    double arg = atof(*argStr);

    //v8::String::AsciiValue arg2(args[0]->ToString());
    //std::cout << *arg1 << std::endl;

    return Number::New(log10(arg));
}

v8::Handle<v8::Value> log(const v8::Arguments& args)
{
    // Returns the natural logarithm of x

    v8::String::AsciiValue argStr(args[0]->ToString());
    //std::cout << *argStr << std::endl;
    double arg = atof(*argStr);

    //v8::String::AsciiValue arg2(args[0]->ToString());
    //std::cout << *arg1 << std::endl;

    return Number::New(log(arg));
}

v8::Handle<v8::Value> sin(const v8::Arguments& args)
{
    // Returns the natural logarithm of x

    v8::String::AsciiValue argStr(args[0]->ToString());
    //std::cout << *argStr << std::endl;
    double arg = atof(*argStr);

    //v8::String::AsciiValue arg2(args[0]->ToString());
    //std::cout << *arg1 << std::endl;

    return Number::New(sin(arg));
}

v8::Handle<v8::Value> cos(const v8::Arguments& args)
{
    // Returns the natural logarithm of x

    v8::String::AsciiValue argStr(args[0]->ToString());
    //std::cout << *argStr << std::endl;
    double arg = atof(*argStr);

    //v8::String::AsciiValue arg2(args[0]->ToString());
    //std::cout << *arg1 << std::endl;

    return Number::New(cos(arg));
}

v8::Handle<v8::Value> tan(const v8::Arguments& args)
{
    // Returns the natural logarithm of x

    v8::String::AsciiValue argStr(args[0]->ToString());
    //std::cout << *argStr << std::endl;
    double arg = atof(*argStr);

    //v8::String::AsciiValue arg2(args[0]->ToString());
    //std::cout << *arg1 << std::endl;

    return Number::New(tan(arg));
}

v8::Handle<v8::Value> randfloat(const v8::Arguments& args)
{
    return Number::New(dist(gen));
}

v8::Handle<v8::Value> sleep(const v8::Arguments& args)
{
    // Returns the natural logarithm of x

    v8::String::AsciiValue argStr(args[0]->ToString());
    //std::cout << *argStr << std::endl;
    int msecs = atoi(*argStr);

    boost::this_thread::sleep(boost::posix_time::milliseconds(msecs));

    //v8::String::AsciiValue arg2(args[0]->ToString());
    //std::cout << *arg1 << std::endl;

    return v8::Undefined();
}


v8::Handle<v8::Value> load(const v8::Arguments& args)
{
    // Returns the natural logarithm of x

    v8::String::AsciiValue argStr(args[0]->ToString());
    std::cout << "Supposed to load this: " << *argStr << std::endl;
    //int msecs = atoi(*argStr);

    //boost::this_thread::sleep(boost::posix_time::milliseconds(msecs));

    //v8::String::AsciiValue arg2(args[0]->ToString());
    //std::cout << *arg1 << std::endl;

    return v8::Undefined();
}

v8::Handle<v8::Value> timems(const v8::Arguments& args)
{
    // Returns the natural logarithm of x

    //v8::String::AsciiValue argStr(args[0]->ToString());
    //std::cout << *argStr << std::endl;
    //int msecs = atoi(*argStr);

    //boost::this_thread::sleep(boost::posix_time::milliseconds(msecs));

    //boost::posix_time::ptime t = boost::posix_time::microsec_clock::universal_time();

    //v8::String::AsciiValue arg2(args[0]->ToString());
    //std::cout << *arg1 << std::endl;

    boost::posix_time::ptime epoch = boost::posix_time::time_from_string("1970-01-01 00:00:00.000");
    boost::posix_time::ptime other = boost::posix_time::microsec_clock::universal_time();

    long diff = (other-epoch).total_milliseconds();

    return Number::New(diff);
}




v8::Handle<Value> ConsoleMethod_Log(const Arguments& args)
{
    Local<Object> self = args.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Console*>(ptr)->Log(args[0]->ToString());

    return v8::Undefined();
}

class RaptorAPI {
    public:
        RaptorAPI() : version("Quant-JS Engine, 0.01 Alpha") {};

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
    double underlyingDouble = 0.0;
    double strikeDouble = 0.0;
    double timeToMaturityYears = 0.0;
    double dividendYieldDouble = 0.0;
    double riskFreeRateDouble = 0.0;
    double volatilityDouble = 0.0;

    if (args[0]->IsObject()) {
        Handle<Object> object = Handle<Object>::Cast(args[0]);

        //// Get parameters from arguments
        Handle<Value> optionType = object->Get(String::New("optionType"));
        Handle<Value> underlyingPrice = object->Get(String::New("underlyingPrice"));
        Handle<Value> strikePrice = object->Get(String::New("strikePrice"));
        Handle<Value> timeToMaturity = object->Get(String::New("timeToMaturity"));
        Handle<Value> dividendYield = object->Get(String::New("dividendYield"));
        Handle<Value> riskFreeRate = object->Get(String::New("riskFreeRate"));
        Handle<Value> volatility = object->Get(String::New("volatility"));

        v8::String::AsciiValue optionTypeStr(optionType->ToString());

        if (underlyingPrice->IsNumber()) {
            Handle<Number> underlyingPriceNum = Handle<Number>::Cast(underlyingPrice);
            v8::String::AsciiValue testValue(underlyingPriceNum->ToString());
            underlyingDouble = atof(*testValue);
        }

        if (strikePrice->IsNumber()) {
            Handle<Number> strikePriceNum = Handle<Number>::Cast(strikePrice);
            v8::String::AsciiValue testValue(strikePriceNum->ToString());
            strikeDouble = atof(*testValue);
        }

        if (timeToMaturity->IsNumber()) {
            Handle<Number> timeToMaturityNum = Handle<Number>::Cast(timeToMaturity);
            v8::String::AsciiValue testValue(timeToMaturityNum->ToString());
            timeToMaturityYears = atof(*testValue);
        }

        if (dividendYield->IsNumber()) {
            Handle<Number> dividendYieldNum = Handle<Number>::Cast(dividendYield);
            v8::String::AsciiValue testValue(dividendYieldNum->ToString());
            dividendYieldDouble = atof(*testValue);
        }

        if (riskFreeRate->IsNumber()) {
            Handle<Number> riskFreeRateNum = Handle<Number>::Cast(riskFreeRate);
            v8::String::AsciiValue testValue(riskFreeRateNum->ToString());
            riskFreeRateDouble = atof(*testValue);
        }

        if (volatility->IsNumber()) {
            Handle<Number> volatilityNum = Handle<Number>::Cast(volatility);
            v8::String::AsciiValue testValue(volatilityNum->ToString());
            volatilityDouble = atof(*testValue);
        }
    }

    //// Method for option pricing
    v8::String::AsciiValue pricingMethod(args[1]->ToString());
    std::cout << "Method: " << *pricingMethod << std::endl;

    //// Option kind
    v8::String::AsciiValue optionKind(args[2]->ToString());
    std::cout << "Kind: " << *optionKind << std::endl;

    // TODO: Implement check for supported pricing functions for each kind

    //// Setup dates
    QuantLib::Calendar calendar = QuantLib::TARGET();
    QuantLib::Date todaysDate = QuantLib::Date::todaysDate(); //todaysDate(24, QuantLib::October, 2013);
    QuantLib::Date settlementDate = QuantLib::Date::todaysDate(); //(24, QuantLib::October, 2013);
    QuantLib::Settings::instance().evaluationDate() = todaysDate;

    //// The options
    QuantLib::Option::Type type(QuantLib::Option::Put);
    QuantLib::Real underlying = underlyingDouble;
    QuantLib::Real strike = strikeDouble;
    QuantLib::Spread dividendYield = dividendYieldDouble;
    QuantLib::Rate riskFreeRate = riskFreeRateDouble;
    QuantLib::Volatility volatility = volatilityDouble;
    // 2013-10-18
    //QuantLib::Date maturity(18, QuantLib::November, 2013);
    QuantLib::Date maturity = QuantLib::Date::todaysDate();
    maturity += 365 * timeToMaturityYears * QuantLib::Days;
    QuantLib::DayCounter dayCounter = QuantLib::Actual365Fixed();

    //// Black-Scholes for European
    // TODO: Don't init all if not used

    std::vector<QuantLib::Date> exerciseDates;
    for (QuantLib::Integer i=1; i<=4; i++) {
        exerciseDates.push_back(settlementDate + 3*i*QuantLib::Months);
    }

    boost::shared_ptr<QuantLib::Exercise> europeanExercise(new QuantLib::EuropeanExercise(maturity));

    boost::shared_ptr<QuantLib::Exercise> bermudanExercise(new QuantLib::BermudanExercise(exerciseDates));

    boost::shared_ptr<QuantLib::Exercise> americanExercise(new QuantLib::AmericanExercise(settlementDate, maturity));

    boost::shared_ptr<QuantLib::StrikedTypePayoff> payoff(new QuantLib::PlainVanillaPayoff(type, strike));

    QuantLib::VanillaOption europeanOption(payoff, europeanExercise);
    QuantLib::VanillaOption bermudanOption(payoff, bermudanExercise);
    QuantLib::VanillaOption americanOption(payoff, americanExercise);

    QuantLib::Handle<QuantLib::Quote> underlyingH(boost::shared_ptr<QuantLib::Quote>(new QuantLib::SimpleQuote(underlying)));
    QuantLib::Handle<QuantLib::YieldTermStructure> flatTermStructure(boost::shared_ptr<QuantLib::YieldTermStructure>(new QuantLib::FlatForward(settlementDate, riskFreeRate, dayCounter)));
    QuantLib::Handle<QuantLib::YieldTermStructure> flatDividendTS(boost::shared_ptr<QuantLib::YieldTermStructure>(new QuantLib::FlatForward(settlementDate, dividendYield, dayCounter)));
    QuantLib::Handle<QuantLib::BlackVolTermStructure> flatVolTS(boost::shared_ptr<QuantLib::BlackVolTermStructure>(new QuantLib::BlackConstantVol(settlementDate, calendar, volatility, dayCounter)));

    // TODO: Clean up here, and lift to other file/class

    if (strncmp(*pricingMethod, "Black-Scholes", strlen("Black-Scholes") - 1) == 0) {
         // Black-Scholes
        boost::shared_ptr<QuantLib::BlackScholesMertonProcess> bsmProcess(new QuantLib::BlackScholesMertonProcess(underlyingH, flatDividendTS,flatTermStructure, flatVolTS));
        europeanOption.setPricingEngine(boost::shared_ptr<QuantLib::PricingEngine>(new QuantLib::AnalyticEuropeanEngine(bsmProcess)));
        return Number::New(europeanOption.NPV());

    } else if (strncmp(*pricingMethod, "Heston semi-analytic", strlen("Heston semi-analytic") - 1) == 0) {
        // Heston semi-analytic
        boost::shared_ptr<QuantLib::HestonProcess> hestonProcess(new QuantLib::HestonProcess(flatTermStructure, flatDividendTS, underlyingH, volatility*volatility, 1.0, volatility*volatility, 0.001, 0.0));
        boost::shared_ptr<QuantLib::HestonModel> hestonModel(new QuantLib::HestonModel(hestonProcess));
        europeanOption.setPricingEngine(boost::shared_ptr<QuantLib::PricingEngine>(new QuantLib::AnalyticHestonEngine(hestonModel)));
        return Number::New(europeanOption.NPV());

    } else if (strncmp(*pricingMethod, "Binomial Trigeorgis", strlen("Binomial Trigeorgis") - 1) == 0) {
        // Binomial Trigeorgis
        // Works for all three options kinds

        QuantLib::Size timeSteps = 801;

        if (strncmp(*optionKind, "European", strlen("European") - 1) == 0) {
            boost::shared_ptr<QuantLib::BlackScholesMertonProcess> bsmProcess(new QuantLib::BlackScholesMertonProcess(underlyingH, flatDividendTS,flatTermStructure, flatVolTS));
            europeanOption.setPricingEngine(boost::shared_ptr<QuantLib::PricingEngine>(new QuantLib::BinomialVanillaEngine<QuantLib::Trigeorgis>(bsmProcess, timeSteps)));
            return Number::New(europeanOption.NPV());
        } else if (strncmp(*optionKind, "American", strlen("American") - 1) == 0) {
            //QuantLib::VanillaOption europeanOption(payoff, europeanExercise);
            boost::shared_ptr<QuantLib::BlackScholesMertonProcess> bsmProcess(new QuantLib::BlackScholesMertonProcess(underlyingH, flatDividendTS,flatTermStructure, flatVolTS));
            americanOption.setPricingEngine(boost::shared_ptr<QuantLib::PricingEngine>(new QuantLib::BinomialVanillaEngine<QuantLib::Trigeorgis>(bsmProcess, timeSteps)));
            return Number::New(americanOption.NPV());
        } else if (strncmp(*optionKind, "Bermudan", strlen("Bermudan") - 1) == 0) {
            boost::shared_ptr<QuantLib::BlackScholesMertonProcess> bsmProcess(new QuantLib::BlackScholesMertonProcess(underlyingH, flatDividendTS,flatTermStructure, flatVolTS));
            bermudanOption.setPricingEngine(boost::shared_ptr<QuantLib::PricingEngine>(new QuantLib::BinomialVanillaEngine<QuantLib::Trigeorgis>(bsmProcess, timeSteps)));
            return Number::New(bermudanOption.NPV());
        }

        return Number::New(-1.0);
    }

    return Number::New(-1.0);
}

Handle<Value> Raptor_Subscribe(const Arguments& args) {

    Local<Object> self = args.Holder();
    Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();

    static_cast<RaptorAPI*>(ptr)->Subscribe();

    Handle<Function> func = Handle<Function>::Cast(args[2]);
    Handle<Value> argv[0];
    Handle<Value> js_result = func->Call(self, 0, argv);

    return v8::Undefined();
}


#define QJS_VERSION "0.0.4"

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

class ARCore {
public:
	ARCore(Options options) : options(options) {}
	void run();
private:
	const Options& options;
};

v8::Handle<v8::Value> Print2(const v8::Arguments& args) {
	v8::HandleScope scope;

	for (int i = 0; i < args.Length(); i++) {
		v8::Handle<v8::String> str = args[i]->ToString();
		v8::String::AsciiValue ascii(str);

		std::cout << *ascii;

	}

	return v8::Undefined();
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

   	Handle<FunctionTemplate> raptor_templ = FunctionTemplate::New();
    raptor_templ->SetClassName(String::New("QuantJS API"));

    Handle<ObjectTemplate> raptor_proto = raptor_templ->PrototypeTemplate();

    raptor_proto->Set("version", FunctionTemplate::New(Raptor_Version));
    raptor_proto->Set("subscribe", FunctionTemplate::New(Raptor_Subscribe));

    raptor_proto->Set("Print", FunctionTemplate::New(Print));
    raptor_proto->Set("eqtest", FunctionTemplate::New(Equity_Option_01));

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


    Handle<ObjectTemplate> raptor_inst = raptor_templ->InstanceTemplate();
    raptor_inst->SetInternalFieldCount(1);

    RaptorAPI* raptor = new RaptorAPI();
    Handle<Function> raptor_ctor = raptor_templ->GetFunction();
    Local<Object> raptor_obj = raptor_ctor->NewInstance();
    raptor_obj->SetInternalField(0, External::New(raptor));

	context->Global()->Set(String::New("quantjs"), raptor_obj);

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
		
    Handle<Value> befunc = context->Global()->Get(String::New("init"));
    Handle<Function> func = Handle<Function>::Cast(befunc);
    Handle<Value> args[0];

    Handle<Value> js_result = func->Call(context->Global(), 0, args);	

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
