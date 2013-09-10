var a=1;

// TODO: Adapt for QuantLib
// Add Option object, with parameters, then send to various pricing functions

function OptionParams(optionType, underlyingPrice, strikePrice, dividendYield, riskFreeRate, volatility) { 
	this.optionType = optionType;
	this.underlyingPrice = underlyingPrice;
	this.strikePrice = strikePrice;
	this.dividendYield = dividendYield;
	this.riskFreeRate = riskFreeRate;
	this.volatility = volatility;
}

function testParams() {
	Print(new OptionParams("PUT", 36, 40, 0.00, 0.06, 0.20));

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

testParams();