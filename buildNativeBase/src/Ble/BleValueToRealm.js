// bleValueToRealmDatabase.js
import React, { Component } from 'react';
import { connect, ble } from '../Redux/'
import { realmMeasureService } from '../Realm/'
import { Buffer } from 'buffer'
import { heartRate, Battery, devInfo, customService, pedometerService, deviceUUID } from './config.js'



const Utils = {
	
	readValueToHex: function(value){
		if (!!value) {
	    // valueAscii = new Buffer(value.toString(), "base64").toString('ascii')
	  	let valueHex = new Buffer(value.toString(), "base64").toString('hex')
	  	return valueHex
	  } else {
	  	return null
	  }
	},

	processHeartRateAndSpO2Instance: function(numberHex:string){
		if(numberHex.length%2 === 0){
			let arr = numberHex.match(/.{1,2}/g);
			let arrDec = arr.map((ar)=>{
				return parseInt(ar,16)
			})
			let len1 = arrDec[1]
			let len2 = arrDec[2]
			arrDec.splice(0, 3)
			arrDec.splice(len1+len2, arrDec.length-(len1+len2))
			let arrHR = arrDec.splice(0, len1)
			return ({
				heartRate: arrHR,
				spO2: arrDec
			})
		}
		return null
	},

	processHeartRateAndSpO2Value: function(numberHex:string){
		if(numberHex.length%2 === 0){
			let arr = numberHex.match(/.{1,2}/g);
			let arrDec = arr.map((ar)=>{
				return parseInt(ar,16)
			})
			arrDec.splice(0, 1)
			return ({
				heartRate: arrDec[0],
				spO2: arrDec[1]
			})
		}
		return null
	},

	processPedometerValue: function(numberHex:string){
		if(numberHex.length%2 === 0){
			let arr = numberHex.match(/.{1,2}/g);
			let arrDec = arr.map((ar)=>{
				return parseInt(ar,16)
			})
			return ({
				stepsCount: arrDec[0],
				distance: arrDec[1],
				caloriesBurn: arrDec[2]
			})
		}
		return null
	}
}



class BleValueToRealm extends Component {
	constructor(props){
		super()
	}

	componentWillReceiveProps(newProps){
		
		let isNotifiable, isNotifying, isReadable, isWritable, value, valueHex;

		// Handle Heart Rate Location
		if(newProps.HRLocation !== this.props.HRLocation){
			if(!!newProps.HRLocation){
				isReadable = newProps.HRLocation.get('isReadable')
				if(isReadable){
					value = newProps.HRLocation.get('value')
					if(!!value){
						valueHex = Utils.readValueToHex(value)
						// realmMeasureService.saveInstance('InstanceStepCount', valueDec)
						// this.props.updateStates('HRLocation')
					}
				}
			}
		}

		// Handle Heart Rate Measurement
		// Read don't need diff
		if(!!newProps.HeartRate){ // prevent undefined, null
		  isNotifying = newProps.HeartRate.get('isNotifying')
		  if(!!isNotifying){
		  	value = newProps.HeartRate.get('value')
	      if(!!value){
		      valueHex= Utils.readValueToHex(value)
		      if(!!valueHex){
			      let object = Utils.processHeartRateAndSpO2Instance(valueHex)
			      if(!!object){
				      realmMeasureService.insertManyInstance('InstanceHeartRate', object.heartRate)
				      realmMeasureService.insertManyInstance('InstanceSpO2', object.spO2)
							this.props.updateStates('InstanceHeartRate')
			      }
		      }
	      }
		  }
		}

		// Handle Heart Rate Value
		// Read don't need diff 
		if(!!newProps.HeartRateValue){ // prevent undefined, null
		  isReadable = newProps.HeartRateValue.get('isReadable')
			if(!!isReadable){
		  	value = newProps.HeartRateValue.get('value')
	      if(!!value){
		      valueHex= Utils.readValueToHex(value)
		      if(!!valueHex){
			      let object = Utils.processHeartRateAndSpO2Value(valueHex)
			      if(!!object){
				      realmMeasureService.save('HeartRateMeasurement', object)
				      this.props.updateStates('HeartRateMeasurement')
			      }
		      }
	      }
		  }
		}

		// Handle Pedometer Measurement
		// Read don't need diff 		
		if(!!newProps.Pedometer){ // prevent undefined, null			
			console.log('has read')
		  isReadable = newProps.Pedometer.get('isReadable')
			if(!!isReadable){
		  	value = newProps.Pedometer.get('value')
	      if(!!value){
		      valueHex= Utils.readValueToHex(value)
		      if(!!valueHex){
			      let object = Utils.processPedometerValue(valueHex)
			      if(!!object){
				      realmMeasureService.save('PedometerMeasurement', object)
				      this.props.updateStates('PedometerMeasurement')
			      }
		      }
	      }
		  }
		}

	}

	render() {
		return (null);
	}
}

export default connect(
  state => ({
    HRLocation: state.getIn(['ble', 'devices', deviceUUID, 'services', heartRate.services, 'characteristics', heartRate.location]),
    HeartRate: state.getIn(['ble', 'devices', deviceUUID, 'services', heartRate.services, 'characteristics', heartRate.measurement]),
    HeartRateValue: state.getIn(['ble', 'devices', deviceUUID, 'services', heartRate.services, 'characteristics', heartRate.measValue]),
    Pedometer: state.getIn(['ble', 'devices', deviceUUID, 'services', pedometerService.services, 'characteristics', pedometerService.measurement])
  }),
  {
    
  	readCharacteristic: ble.readCharacteristic,
  	writeCharacteristic: ble.writeCharacteristic,
  	monitorCharacteristic: ble.monitorCharacteristic,
    pushError: ble.pushError,
    updateStates: ble.updateStates
  })
(BleValueToRealm)