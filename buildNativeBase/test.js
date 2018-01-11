// test.js
'use strict';
const {Map, List, OrderedMap} = require('Immutable')

let keysOfEventsUpdate = ['HRmeas', 'HRlocat']

const defaultStates = Map({
	updateStates: List([true, true])
})
const defaultStates1 = defaultStates.set('updateStates', defaultStates.get('updateStates').update(0, val =>{
	return (!val)
}));
// console.log(defaultStates1.get('updateStates'))
// 
// 
var {Buffer} = require('buffer')

let value = "PQ=="
let valueHex = new Buffer(value.toString(), "base64").toString().charCodeAt(0)
console.log(valueHex)