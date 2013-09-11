var a=1;

// TODO: Adapt for QuantLib
// Add Option object, with parameters, then send to various pricing functions
// PricingEngine, various pricing methods, then do engine.npv(new OptionParams(...))
// European, Bermudan and American

function OptionParams(optionType, underlyingPrice, strikePrice, dividendYield, riskFreeRate, volatility) { 
	this.optionType = optionType;
	this.underlyingPrice = underlyingPrice;
	this.strikePrice = strikePrice;
	this.dividendYield = dividendYield;
	this.riskFreeRate = riskFreeRate;
	this.volatility = volatility;
}

// TODO print method

// Class definition / PricingEngine
var PricingEngine = function PricingEngine(method) {

	// Check if method is supported
	this.methods = ["Black-Scholes", "Heston semi-analytic", "Binomial Trigeorgis"];

	if (method in this.methods) {
		this.method = method;
	} else {
		console.log("Method not supported");
	}
}

// Instance methods
PricingEngine.prototype = {
	constructor: PricingEngine,
	calculateNPV: function calculateNPV(optionParams) {
		if (this.method != null) {

			// Just a hack for now, make more robust, method objects and enums etc.
			console.log(this.methods[this.method]);

			return raptor.eqtest(optionParams, this.methods[this.method]);

		} else {
			console.log("Method not specified");
		}
	},
	listMethods: function listMethods() {
		this.methods.forEach( function(m) {
			console.log(m);
		});
	}
}

// var pricingEngine = new PricingEngine(1);
// var optionParams = new OptionParams("PUT", 36, 40, 0.00, 0.06, 0.20);
// pricingEngine.calculateNPV(optionParams);

function testNPV(val) {
	var pricingEngine = new PricingEngine(val);
	var optionParams = new OptionParams("PUT", 36, 40, 0.00, 0.06, 0.20);
	return pricingEngine.calculateNPV(optionParams);
}

/*
function testNPV1() {
	var pricingEngine = new PricingEngine(0);
	var optionParams = new OptionParams("PUT", 36, 40, 0.00, 0.06, 0.20);
	return pricingEngine.calculateNPV(optionParams);
}

function testNPV2() {
	var pricingEngine = new PricingEngine(2);
	var optionParams = new OptionParams("PUT", 36, 40, 0.00, 0.06, 0.20);
	return pricingEngine.calculateNPV(optionParams);
}
*/

function testParams() {
	raptor.Print(new OptionParams("PUT", 36, 40, 0.00, 0.06, 0.20));

	// Rough test of running pricing model, simple BS
	// maturity(17, QuantLib::May, 1999);
	// QuantLib::Date todaysDate(15, QuantLib::May, 1998);
	raptor.eqtest(new OptionParams("PUT", 36, 40, 0.00, 0.06, 0.20));

}

// Initialize
function init() {
	console.log('Initializing algo APAN');

	console.log(raptor.version());

	// Subscribe to market data
	raptor.subscribe('EUR/USD', tickHandler, function (data, err) {
		console.log('Successfully subscribed to market data');
			/*
			if (err) {
				console.log("Order Error", err.message);
			} else {
				console.log("Order Sent", data.ticket);
			}*/
		//});
	});
}

function tickHandler(data) {
	console.log('das');

	//console.log(data.ask);

	//console.log("Symbol: ", data.symbol, " -- Bid: ", data.bid, " -- Ask: ", data.ask);

	if (1 == 1) {
		//OrderSend(Symbol(),OP_BUY,1,Ask,3,Ask-25*Point,Ask+25*Point,"My order #2",16384,0,Green);
	}
}

// Cleanup
function deinit() {
	console.log('Cleaning up alog');
}

//testParams();