
'use strict';

var { Buffer } = require('buffer');

var value = 45;

var valueAscii = new Buffer(value.toString(), "base64").toString('ascii')

console.log(valueAscii)