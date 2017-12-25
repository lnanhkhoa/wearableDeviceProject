// config.js
module.exports.deviceUUID = 'CC:78:AB:AD:F2:07'
module.exports.servicesUUID = servicesUUID = {
	keys:[
		'180a',
		'180f',
		'180d',
		'fff0',
		'ffe0'
	]
}
module.exports.heartRate = heartRate = {
	services: 		'0000180d-0000-1000-8000-00805f9b34fb',
	location: 		'00002a38-0000-1000-8000-00805f9b34fb',
	measValue: 		'00002a3a-0000-1000-8000-00805f9b34fb',
	measurement: 	'00002a3b-0000-1000-8000-00805f9b34fb',
	controlpoint: '00002a39-0000-1000-8000-00805f9b34fb'
}
module.exports.Battery = Battery = {
	level: 		'00002a19-0000-1000-8000-00805f9b34fb',
	services: '0000180f-0000-1000-8000-00805f9b34fb'
}
module.exports.devInfo = devInfo = {
	services: 				'0000180a-0000-1000-8000-00805f9b34fb',
	modelNumber: 			'00002a24-0000-1000-8000-00805f9b34fb',
	manufacturerName: '00002a29-0000-1000-8000-00805f9b34fb',
	firmwareRevision: '00002a26-0000-1000-8000-00805f9b34fb',
	manufacturerValue:'MEMSTECH'
}
module.exports.customService = customService = {
	services: '0000fff0-0000-1000-8000-00805f9b34fb',
	char1: 		'0000fff1-0000-1000-8000-00805f9b34fb',
	char2: 		'0000fff2-0000-1000-8000-00805f9b34fb',
	char3: 		'0000fff3-0000-1000-8000-00805f9b34fb',
	char4: 		'0000fff4-0000-1000-8000-00805f9b34fb',			
	char5: 		'0000fff5-0000-1000-8000-00805f9b34fb'
}
module.exports.pedometerService = pedometerService = {
	services: 		'0000ffe0-0000-1000-8000-00805f9b34fb',
	measurement: 	'0000ffe1-0000-1000-8000-00805f9b34fb',
	char2: 				'0000ffe2-0000-1000-8000-00805f9b34fb'
}
