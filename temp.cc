
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
