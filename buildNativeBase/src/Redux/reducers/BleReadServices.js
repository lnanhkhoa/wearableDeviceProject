// BleReadServices.js
import React, { Component } from 'react'

let ReadServicesAllReadable = {
	
	findAll: function (ServiceUUID) {
		
	},

	listCharacteristicsUUID: function(ServiceUUID){

	}
}



class BleReadServices extends Component {
	render() {
		return null;
	}
}


export default connect(
  state => ({
    operations: state.getIn(['ble', 'operations']),
    scanning: state.getIn(['ble', 'scanning']),
    state: state.getIn(['ble', 'state']),
    selectedDeviceId: state.getIn(['ble', 'selectedDeviceId'])
  }),
  {
    deviceFound: ble.deviceFound,
    changeDeviceState: ble.changeDeviceState,
    serviceIdsForDevice: ble.serviceIdsForDevice,
    stopScan: ble.stopScan,
    updateServices: ble.updateServices,
    updateCharacteristic: ble.updateCharacteristic,
    executeTransaction: ble.executeTransaction,
    completeTransaction: ble.completeTransaction,
    pushError: ble.pushError
  })
  (BleReadServices)
