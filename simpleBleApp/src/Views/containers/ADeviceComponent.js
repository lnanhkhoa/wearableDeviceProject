// ADeviceComponent.js
// 
 
 import React, { Component } from 'react'
 import { View, Text, StyleSheet, TouchableOpacity } from 'react-native'
import { connect } from 'react-redux'
import { Actions } from 'react-native-router-flux'

import * as SceneConst from '../../Redux/reducers/Const.js'
import * as ble from '../../Redux/reducers/BleActions.js'
import Style from '../Style.js'

class ADeviceComponent extends Component {
 	
 	_onPressButtonDisconnectDevice=()=>{
 		this.props.changeDeviceState(this.props.selectedDeviceId, ble.DEVICE_STATE_DISCONNECT);
 		Actions.pop();
 	}

 	render() {
 		return (
 			<View style={Style.component}>
 				<TouchableOpacity onPress={this._onPressButtonDisconnectDevice}>
 				<Text> Disconnect Device </Text>
	      </TouchableOpacity>
 			</View>
 		);
 	}
 }
 
const mapStateToProps=(state) =>({
	selectedDeviceId: state.getIn(['ble', 'selectedDeviceUUID']),
  sceneState: state.getIn(['route', 'state'])
})

const mapDispatchToProps = {
	changeDeviceState: ble.changeDeviceState
}

 export default connect(mapStateToProps, mapDispatchToProps)(ADeviceComponent)
