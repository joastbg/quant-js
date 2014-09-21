
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


// NEW



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

v8::Handle<v8::Value> require(const v8::Arguments& args) {


	v8::String::AsciiValue argStr(args[0]->ToString());

	Handle<v8::Object> global = context->Global();

	v8::Handle<v8::String> source = ReadFile(*argStr);
	Handle<Script> script = Script::Compile(source);
	Handle<Value> result = script->Run();

	std::cout << "Apan rockar!\n";

	return v8::Undefined();
}


v8::Handle<v8::Value> glambda_proxy(const v8::Arguments& args) {
	v8::HandleScope scope;

	std::cout << "Apan rockar!\n";

	return v8::Undefined();
}

void fjohan() {
	std::cout << "From function fjohan\n";
}

void fnisse() {
	std::cout << "From function fnisse\n";
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
#include <thread>
#include <chrono>
#include <pthread.h>

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/thread/thread.hpp>

#include <boost/random.hpp>      
#include <boost/limits.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include <Eigen/Eigenvalues>

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

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
 
#include <GL/glew.h>
#include <GL/glxew.h>
#include <GL/freeglut.h>


// Colors
GLfloat WHITE[] = {1, 1, 1};
GLfloat RED[] = {1, 0, 0};
GLfloat GREEN[] = {0, 1, 0};
GLfloat MAGENTA[] = {1, 0, 1};

// A camera.  It moves horizontally in a circle centered at the origin of
// radius 10.  It moves vertically straight up and down.
class Camera {
  double theta;      // determines the x and z positions
  double y;          // the current y position
  double dTheta;     // increment in theta for swinging the camera around
  double dy;         // increment in y for moving the camera up/down
public:
  Camera(): theta(0), y(3), dTheta(0.04), dy(0.2) {}
  double getX() {return 10 * cos(theta);}
  double getY() {return y;}
  double getZ() {return 10 * sin(theta);}
  void moveRight() {theta += dTheta;}
  void moveLeft() {theta -= dTheta;}
  void moveUp() {y += dy;}
  void moveDown() {if (y > dy) y -= dy;}
};

class Checkerboard {
  int displayListId;
  int width;
  int depth;
public:
  Checkerboard(int width, int depth): width(width), depth(depth) {}
  double centerx() {return width*0.25 / 2;}
  double centerz() {return depth*0.25 / 2;}
  void create() {
    displayListId = glGenLists(1);
    glNewList(displayListId, GL_COMPILE);
    GLfloat lightPosition[] = {4, 3, 7, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glBegin(GL_QUADS);
    glNormal3d(0, 1, 0);
    for (int x = 0; x < width - 1; x++) {
      for (int z = 0; z < depth - 1; z++) {
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                     (x + z) % 2 == 0 ? RED : WHITE);
        glVertex3d((x)*0.25, 0, (z)*0.25);
        glVertex3d((x+1)*0.25, 0, (z)*0.25);
        glVertex3d((x+1)*0.25, 0, (z+1)*0.25);
        glVertex3d((x)*0.25, 0, (z+1)*0.25);
      }
    }
    glEnd();
    glEndList();
  }
  void draw() {
    glCallList(displayListId);
  }
};

Checkerboard checkerboard(256, 256);
Camera camera;

void init() {
  glEnable(GL_DEPTH_TEST);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
  glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
  glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
  glMaterialf(GL_FRONT, GL_SHININESS, 30);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  checkerboard.create();
}

// Draws one frame, the checkerboard then the balls, from the current camera
// position.
void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
            checkerboard.centerx(), 0.0, checkerboard.centerz(),
            0.0, 1.0, 0.0);
  checkerboard.draw();
  //for (int i = 0; i < sizeof balls / sizeof(Ball); i++) {
  //  balls[i].update();
  //}
  glFlush();
  glutSwapBuffers();
}

// On reshape, constructs a camera that perfectly fits the window.
void reshape(GLint w, GLint h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, GLfloat(w) / GLfloat(h), 1.0, 150.0);
  glMatrixMode(GL_MODELVIEW);
}

// Requests to draw the next frame.
void timer(int v) {
  glutPostRedisplay();
  glutTimerFunc(1000/60, timer, v);
}

// Moves the camera according to the key pressed, then ask to refresh the
// display.
void special(int key, int, int) {
  switch (key) {
    case GLUT_KEY_LEFT: camera.moveLeft(); break;
    case GLUT_KEY_RIGHT: camera.moveRight(); break;
    case GLUT_KEY_UP: camera.moveUp(); break;
    case GLUT_KEY_DOWN: camera.moveDown(); break;
  }
  glutPostRedisplay();
}

struct gl_visualization {
    void run() {

        char* buffer = (char*)"quant-js";
        int argc=1;
        char** argv = &buffer;

        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowPosition(80, 80);
        glutInitWindowSize(800, 600);
        glutCreateWindow(QJS_VERSION);
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutSpecialFunc(special);
        glutTimerFunc(100, timer, 0);
        init();
        glutMainLoop();

        /*
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
        glutInitWindowSize(640, 480);
        glutInitWindowPosition(100, 100);
        glutCreateWindow(QJS_VERSION);
        glutDisplayFunc(dispfun);
        glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
        glutMainLoop();
        */

        for (;;) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "gl-viz run...\n";
        }
    }
};

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

    // A. Define the lagged Fibonacci and Uniform objects
    boost::lagged_fibonacci607 rng;
    rng.seed(static_cast<int> (std::time(0)));
    boost::uniform_real<> uni(0.0,1.0);
     
    // B. Produce Uniform (0, 1)
    //boost::variate_generator<boost::lagged_fibonacci607&,boost::uniform_real<>> uniRng(rng, uni);
     
    boost::variate_generator<boost::lagged_fibonacci607, 
                           boost::uniform_real<> > uniRng(rng, uni);
    // C. Choose the desired accuracy
    std::cout << "How many darts to throw? "; long N; std::cin >> N;
     
    // D. Thow the dart; does it fall in the circle or outside
    // Start throwing darts and count where they land
    long hits = 0;
    double x, y, distance;
    for (long n = 1; n <= N; ++n) {
        x = uniRng(); y = uniRng();
        distance = sqrt(x*x + y*y);
     
        if ( distance <=1) {
            hits++;
        }
    }
     
    // E.  Produce the answer
    std::cout << "PI is: " << hits << ", " << 4.0 * double(hits) / double (N) << std::endl;
}

void fnisse() {
    std::cout << "From function fnisse\n";
    //draw();
    
    using Eigen::MatrixXcf;
    using Eigen::VectorXcf;
    using Eigen::MatrixXf;
    using Eigen::ComplexEigenSolver;
    using Eigen::IOFormat;    
    using Eigen::StreamPrecision;
    using Eigen::SelfAdjointEigenSolver;

    IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
    IOFormat OctaveFmt(StreamPrecision, 0, ", ", ";\n", "", "", "[", "]");

    MatrixXcf A = MatrixXcf::Random(4,4);
    std::cout << "Here is a random 4x4 matrix, A:" << std::endl << A << std::endl << std::endl;
    ComplexEigenSolver<MatrixXcf> ces;
    ces.compute(A);
    std::cout << "The eigenvalues of A are:" << std::endl << ces.eigenvalues().format(OctaveFmt) << std::endl;
    std::cout << "The matrix of eigenvectors, V, is:" << std::endl << ces.eigenvectors().format(OctaveFmt) << std::endl << std::endl;
    std::complex<float> lambda = ces.eigenvalues()[0];
    std::cout << "Consider the first eigenvalue, lambda = " << lambda << std::endl;
    VectorXcf v = ces.eigenvectors().col(0);
    std::cout << "If v is the corresponding eigenvector, then lambda * v = " << std::endl << lambda * v << std::endl;
    std::cout << "... and A * v = " << std::endl << A * v << std::endl << std::endl;
    std::cout << "Finally, V * D * V^(-1) = " << std::endl
        << (ces.eigenvectors() * ces.eigenvalues().asDiagonal() * ces.eigenvectors().inverse()).format(OctaveFmt)
        << std::endl << std::endl;

    MatrixXcf ones = MatrixXcf::Ones(3,3);
    ComplexEigenSolver<MatrixXcf> ces2(ones);
    std::cout << "The first eigenvector of the 3x3 matrix of ones is:" 
         << std::endl << ces2.eigenvectors().col(1).format(CleanFmt) << std::endl;

    SelfAdjointEigenSolver<MatrixXf> es(4);
    MatrixXf X1 = MatrixXf::Random(4,4);
    MatrixXf A1 = X1 + X1.transpose();
    es.compute(A1);
    std::cout << "The eigenvalues of A are: " << es.eigenvalues().transpose().format(OctaveFmt) << std::endl;
    es.compute(A1 + MatrixXf::Identity(4,4)); // re-use es to compute eigenvalues of A+I
    std::cout << "The eigenvalues of A+I are: " << es.eigenvalues().transpose().format(OctaveFmt) << std::endl;


    Eigen::Matrix3f A2;
    Eigen::Vector3f b2;
    A2 << 1,  3, -2,
           3,  5,  6,
           2,  4,  3;
    b2 << 5, 7, 8;
    std::cout << "Here is the matrix A:\n" << A2 << std::endl;
    std::cout << "Here is the vector b:\n" << b2 << std::endl;
    Eigen::Vector3f x2 = A2.colPivHouseholderQr().solve(b2);
    std::cout << "The solution is:\n" << x2 << std::endl;

    std::cout << "-------------------------------\n";
}

void startgl() {
    gl_visualization foo;
    auto thread = new std::thread(std::bind(&gl_visualization::run, foo));
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
    proxy.add(engine_proto, "startgl", startgl);
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



