// bleValueToRealmDatabase.js
import React, { Component } from 'react';
import { connect, ble } from '../Redux/'
import { realmMeasureService } from '../Realm/'
import { Buffer } from 'buffer'
import { heartRate, Battery, devInfo, customService, deviceUUID } from './config.js'

const readValueToDec = function(value){
	let valueAscii = '0';
	if (!!value) {
    // valueAscii = new Buffer(value.toString(), "base64").toString('ascii')
  	let valueDec = parseInt(new Buffer(value.toString(), "base64").toString('hex'),16)
  	return valueDec
  } else {
  	return null
  }
}

class BleValueToRealm extends Component {
	constructor(props){
		super(props)
	}
	

	componentWillReceiveProps(newProps){
		
		let isNotifiable, isNotifying, isReadable, isWritable, value, valueDec;
		//Handle Heart Rate Measurement
		if(newProps.HRMeasurement !== this.props.HRMeasurement ){
			if(!!newProps.HRMeasurement){ // prevent undefined, null
			  isNotifying = newProps.HRMeasurement.get('isNotifying')
			  if(isNotifying){
			  	value = newProps.HRMeasurement.get('value')
		      if(!!value){
			      valueDec = readValueToDec(value)
				  	realmMeasureService.saveInstance('Instance_HeartRate', valueDec)
				  	realmMeasureService.save
				  	this.props.updateStates('HRMeasurement')
		      }
			  }
			}
		}

		//Handle Heart Rate Location
		if(newProps.HRLocation !== this.props.HRLocation){
			if(!!newProps.HRLocation){
				isReadable = newProps.HRLocation.get('isReadable')
				if(isReadable){
					value = newProps.HRLocation.get('value')
					if(!!value){
						valueDec = readValueToDec(value)
						realmMeasureService.saveInstance('Instance_StepCount', valueDec)
						this.props.updateStates('HRLocation')	
					}
				}
			}
		}

		//Handle Heart Rate Location
		if(newProps.PedoMeasurement !== this.props.PedoMeasurement){
			if(!!newProps.PedoMeasurement){
				isNotifying = newProps.PedoMeasurement.get('isNotifying')
				if(isNotifying){
					value = newProps.PedoMeasurement.get('value')
					if(!!value){
						valueDec = readValueToDec(value)
						realmMeasureService.saveInstance('Instance_StepCount', valueDec)
						this.props.updateStates('PedoMeasurement')	
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
    HRMeasurement: state.getIn(['ble', 'devices', deviceUUID, 'services', heartRate.services, 'characteristics', heartRate.measurement]),
    PedoMeasurement: state.getIn(['ble', 'devices', deviceUUID, 'services', customService.services, 'characteristics', customService.char4]),
  }),
  {
    
  	readCharacteristic: ble.readCharacteristic,
  	writeCharacteristic: ble.writeCharacteristic,
  	monitorCharacteristic: ble.monitorCharacteristic,
    pushError: ble.pushError,
    updateStates: ble.updateStates
  })
(BleValueToRealm)