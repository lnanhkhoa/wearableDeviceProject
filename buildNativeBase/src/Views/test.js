// test.js
var promise1 = require('./promise.js')

var promise = new Promise(function(resolve, reject){
	// resolve('Success');
	reject('error')
});

promise.then(
	function(success){
		console.log(success)

	},
	function(reject){
		console.log(reject)
	}
)
console.log(promise)