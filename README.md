# About Quant-JS #

## What is Quant-JS ##

Quant-JS is an easy to use quantitative finance workbench for JavaScript, combining Google V8 and QuantLib.

Will price options (European, Bermudan and American) using the following methods, where they apply:
- Black-Scholes
- Heston semi-analytic
- Bates semi-analytic
- Barone-Adesi/Whaley
- Bjerksund/Stensland
- Integral
- Finite differences
- Binomial Jarrow-Rudd
- Binomial Cox-Ross-Rubinstein
- Additive equiprobabilities
- Binomial Trigeorgis
- Binomial Tian
- Binomial Leisen-Reimer
- Binomial Joshi
- MC (crude)
- QMC (Sobol)
- MC (Longstaff Schwartz)

## Getting started ##

You need the following to be able to compile quant-js (yum package names):
- QuantLib (QuantLib-devel)
- Boost libraries (boost-devel)
- Google V8 (v8-devel)
- Optional: rlwrap (wrapper for readline)

Build binary:
# make

Run quantjs
# rlwrap -p'1;33' -m ./quantjs quantjs.js underscore.js

The file quantjs.js is a JavaScript library to hide some implementation details and open for use of other JavaScript libraries as well. 

Here's a fully-functional example of how to valuate three different put options using Quant-JS:

    // Calculation date is today's date
	var pricingEngine = new PricingEngine(2); // Binomial Trigeorgis
    
    // Params: optionType, underlyingPrice, strikePrice, timeToMaturity, dividendYield, riskFreeRate, volatility

    var europeanOption = new EuropeanOption();
    europeanOption.setParams(new OptionParams('PUT', 36, 40, 0.5, 0.00, 0.06, 0.20));
    pricingEngine.calculateNPV(europeanOption); // 3.843556981971868

    var americanOption = new AmericanOption();
    americanOption.setParams(new OptionParams('PUT', 36, 40, 0.5, 0.00, 0.06, 0.20));
    pricingEngine.calculateNPV(americanOption); // 4.486461065154719

    var bermudanOption = new BermudanOption();
    bermudanOption.setParams(new OptionParams('PUT', 36, 40, 0.5, 0.00, 0.06, 0.20));
    pricingEngine.calculateNPV(bermudanOption); // 4.360909275428335


## Project status ##
The project is still in early Alpha stage, so there is a lot of missing functionality.

## License and copyright ##

Quant-JS is copyrighted free software made available under the terms
of either the GNU General Public Licence (GPL).

Copyright: (C) 2012-13 by Johan Astborg. All Rights Reserved.
