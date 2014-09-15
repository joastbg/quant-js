var a=1;

// TODO: Adapt for QuantLib
// Add Option object, with parameters, then send to various pricing functions
// PricingEngine, various pricing methods, then do engine.npv(new OptionParams(...))
// European, Bermudan and American

function fromjs() {
	return "quantjs!!!!";
}

/// Wrapper to make functions global, ie motivated by log, mixup with console.log otherwise
function log(n) {
    return quantjs.log(n);
}

function log10(n) {
    return quantjs.log10(n);
}

function log2(n) {
    // TODO: Implement this in C++
    return quantjs.log(n) / quantjs.log(2);
}

function sin(n) {
    return quantjs.sin(n);
}

function cos(n) {
    return quantjs.cos(n);
}

function tan(n) {
    return quantjs.tan(n);
}

function time(fun) {
    // Will meassure execution time
    var start = quantjs.timems();
    fun();
    var end = quantjs.timems();

    return end - start;
}

function sleep(ms) {
    // Will sleep in ms
}

function exit() {
    quantjs.exit();
}


function OptionParams(optionType, underlyingPrice, strikePrice, timeToMaturity, dividendYield, riskFreeRate, volatility) { 
    this.optionType = optionType;
    this.underlyingPrice = underlyingPrice;
    this.strikePrice = strikePrice;
    this.timeToMaturity = timeToMaturity;
    this.dividendYield = dividendYield;
    this.riskFreeRate = riskFreeRate;
    this.volatility = volatility;
}

// Different options, all extend base Option

var Option = function Option(optionKind) {
    this.optionKind = optionKind;
}

Option.prototype = {
    getKind: function getKind() {
        return this.optionKind;
    },
    getParams: function getParams() {
        return this.optionParams;
    },
    setParams: function setParams(optionParams) {
        this.optionParams = optionParams;
    }
}

// 3 supported option types
var AmericanOption = function AmericanOption() {
    //this.setParams(new OptionParams(optionType, underlyingPrice, strikePrice, dividendYield, riskFreeRate, volatility));
}
AmericanOption.prototype = new Option("American");


var EuropeanOption = function EuropeanOption() {
    //this.setParams(new Option("European", new OptionParams(optionType, underlyingPrice, strikePrice, dividendYield, riskFreeRate, volatility));
}
EuropeanOption.prototype = new Option("European");

var BermudanOption = function BermudanOption() {
    //this.setParams(new OptionParams(optionType, underlyingPrice, strikePrice, dividendYield, riskFreeRate, volatility));
}
BermudanOption.prototype = new Option("Bermudan");

// Class definition / PricingEngine
var PricingEngine = function PricingEngine(method) {

    // Check if method is supported
    this.methods = ["Black-Scholes", "Heston semi-analytic", "Binomial Trigeorgis"];

    if (method in this.methods) {
        this.method = method;
    } else {
        console.log('Method not supported');
    }
}

// Instance methods
PricingEngine.prototype = {
    constructor: PricingEngine,
    calculateNPV: function calculateNPV(optionObject) {
        if (this.method != null) {

            // Just a hack for now, make more robust, method objects and enums etc.
            console.log(this.methods[this.method]);
            console.log(optionObject.getKind());

            return quantjs.eqtest(optionObject.getParams(), this.methods[this.method], optionObject.getKind());

        } else {
            console.log('Method not specified');
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

function testNPV() {
    var pricingEngine = new PricingEngine(0); // BS
    
    var europeanOption = new EuropeanOption();
    europeanOption.setParams(new OptionParams('PUT', 36, 40, 0.5, 0.00, 0.06, 0.20));
    var t = pricingEngine.calculateNPV(europeanOption); // 3.843556981971868
    console.log(t);
/*
    var americanOption = new AmericanOption();
    americanOption.setParams(new OptionParams('PUT', 36, 40, 0.5, 0.00, 0.06, 0.20));
    pricingEngine.calculateNPV(americanOption); // 4.486461065154719

    var bermudanOption = new BermudanOption();
    bermudanOption.setParams(new OptionParams('PUT', 36, 40, 0.5, 0.00, 0.06, 0.20));
    var t = pricingEngine.calculateNPV(bermudanOption); // 4.360909275428335
    console.log(t);
*/
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
    quantjs.Print(new OptionParams('PUT', 36, 40, 0.5, 0.00, 0.06, 0.20));

    // Rough test of running pricing model, simple BS
    // maturity(17, QuantLib::May, 1999);
    // QuantLib::Date todaysDate(15, QuantLib::May, 1998);
    quantjs.eqtest(new OptionParams('PUT', 36, 40, 0.5, 0.00, 0.06, 0.20));

}

// Initialize
function init() {
    console.log('Initializing Quant-JS Library, 0.01 Alpha');

    console.log(quantjs.version());

	// load libs
	quantjs.load("underscore-min.js");

	// _.reduce([1, 2, 3], function(memo, num){ return memo + num; }, 0);

    // Subscribe to market data
    /*
    raptor.subscribe('EUR/USD', tickHandler, function (data, err) {
        console.log('Successfully subscribed to market data');
            
            if (err) {
                console.log("Order Error", err.message);
            } else {
                console.log("Order Sent", data.ticket);
            }
        //});
    });*/
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

function testmatrix() {
	var matrix = [];
	for(var i=0; i<9; i++) {
    	matrix[i] = new Array(9);
	}
	return matrix;
}

function ident(size) {
	return _.range(size).map(function(n) { return _.range(size).map(function(m) { return n==m?1:0; }); });
}

function mrand(size) {
	return _.range(size).map(function(n) { return _.range(size).map(function(m) { return quantjs.rnd(); }); });
}

