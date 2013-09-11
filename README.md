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

Here's a fully-functional example of how to valuate a European Put option using Quant-JS:

	var pricingEngine = new PricingEngine(0);
	var optionParams = new OptionParams("PUT", 36, 40, 0.00, 0.06, 0.20);
	pricingEngine.calculateNPV(optionParams);

This will return:
	Method: Black-Scholes
	3.8443077915968398

## Project status ##
The project is still in early Alpha stage, so there is a lot of missing functionality.

## License and copyright ##

Quant-JS is copyrighted free software made available under the terms
of either the GNU General Public Licence (GPL).

Copyright: (C) 2012-13 by Johan Astborg. All Rights Reserved.