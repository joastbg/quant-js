#include <v8.h> 
#include <iostream>
#include <fstream>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <ql/quantlib.hpp>

using namespace v8;
using namespace boost::gregorian;
using namespace boost::posix_time;

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
    static_cast<Console*>(ptr)->Log(args[0]->ToString());

    return v8::Undefined();
}

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
    double underlyingDouble = 0.0;
    double strikeDouble = 0.0;
    double dividendYieldDouble = 0.0;
    double riskFreeRateDouble = 0.0;
    double volatilityDouble = 0.0;

    if (args[0]->IsObject()) {
        Handle<Object> object = Handle<Object>::Cast(args[0]);

        //// Get parameters from arguments
        Handle<Value> optionType = object->Get(String::New("optionType"));
        Handle<Value> underlyingPrice = object->Get(String::New("underlyingPrice"));
        Handle<Value> strikePrice = object->Get(String::New("strikePrice"));
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
    //if (args[1]-IsString()) {
        v8::String::AsciiValue pricingMethod(args[1]->ToString());
        std::cout << "Method: " << *pricingMethod << std::endl;
    //}

    //// Setup dates
    QuantLib::Calendar calendar = QuantLib::TARGET();
    QuantLib::Date todaysDate(15, QuantLib::May, 1998);
    QuantLib::Date settlementDate(17, QuantLib::May, 1998);
    QuantLib::Settings::instance().evaluationDate() = todaysDate;

    //// The options
    QuantLib::Option::Type type(QuantLib::Option::Put);
    QuantLib::Real underlying = underlyingDouble;
    QuantLib::Real strike = strikeDouble;
    QuantLib::Spread dividendYield = dividendYieldDouble;
    QuantLib::Rate riskFreeRate = riskFreeRateDouble;
    QuantLib::Volatility volatility = volatilityDouble;
    QuantLib::Date maturity(17, QuantLib::May, 1999);
    QuantLib::DayCounter dayCounter = QuantLib::Actual365Fixed();

    //// Black-Scholes for European
    boost::shared_ptr<QuantLib::Exercise> europeanExercise(new QuantLib::EuropeanExercise(maturity));

    boost::shared_ptr<QuantLib::StrikedTypePayoff> payoff(new QuantLib::PlainVanillaPayoff(type, strike));

    QuantLib::VanillaOption europeanOption(payoff, europeanExercise);

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
        QuantLib::Size timeSteps = 801;
        boost::shared_ptr<QuantLib::BlackScholesMertonProcess> bsmProcess(new QuantLib::BlackScholesMertonProcess(underlyingH, flatDividendTS,flatTermStructure, flatVolTS));
        europeanOption.setPricingEngine(boost::shared_ptr<QuantLib::PricingEngine>(new QuantLib::BinomialVanillaEngine<QuantLib::Trigeorgis>(bsmProcess, timeSteps)));
        return Number::New(europeanOption.NPV());
    }

    return Number::New(1.0);
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

// Extracts a C string from a V8 Utf8Value.
const char* ToCString(const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
}

int main(int argc, char* argv[]) {
    
    //// Create a stack-allocated handle scope. 
    HandleScope handle_scope;
 
    //// Loading file to context

    std::cout << "Loading file: " << argv[1] << std::endl;
  
    std::ifstream infile (argv[1], std::ios_base::in);
    std::stringstream sstream;
    while (infile.good() && infile.peek() != -1) {
        sstream.put(infile.get());
    }
    infile.close();

    v8::Handle<v8::ObjectTemplate> global = ObjectTemplate::New();
    Persistent<Context> context = Context::New(NULL, global);
    Context::Scope context_scope(context);

    //// Map Console class

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

    //// Read source from file
    Handle<String> source = String::New(sstream.str().c_str());
    
    //// Compile the source code. 
    Handle<Script> script = Script::Compile(source);

    //// Run the script to get the result. 
    Handle<Value> result = script->Run();

    Handle<Value> befunc = context->Global()->Get(String::New("init"));
    Handle<Function> func = Handle<Function>::Cast(befunc);
    Handle<Value> args[0];
  
    Handle<Value> js_result = func->Call(context->Global(), 0, args);

    v8::Local<v8::String> name(v8::String::New("raptor"));

    while (true) {
        char buffer[256];
        printf("> ");
        char* str = fgets(buffer, 256, stdin);
        if (str == NULL) break;
        v8::HandleScope handle_scope;

        Handle<Script> script = Script::Compile(String::New(str), name);
        v8::Handle<v8::Value> result = script->Run();
        
        String::Utf8Value str2(result);
        const char* cstr = ToCString(str2);
        printf("%s\n", cstr);
    }

    return 0;
}
