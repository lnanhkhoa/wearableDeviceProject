// test.js

const {Map, List, OrderedMap} = require('Immutable')


var arr ={
	1:Map({
		name: "V06"
	}
	),
	2:Map({
		name: "V07"
	}
	)	
}
var devices = OrderedMap(arr)
var devicess = devices.toObject()
console.log(devicess)
var keys = Object.keys(devicess)
var results = keys.map(function(v){
	return devices.getIn([v]).toObject()
})
console.log(results)