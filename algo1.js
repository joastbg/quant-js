var a=1;

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
