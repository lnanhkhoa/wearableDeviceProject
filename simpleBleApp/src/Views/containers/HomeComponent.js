 // HomeComponent.js
 
import React, { Component } from 'react'
import { View, Text, StyleSheet } from 'react-native'
import { connect } from 'react-redux'
import { Actions } from 'react-native-router-flux'

import * as ble from '../../Redux/reducers/BleActions'
import * as SceneConst from '../../Redux/reducers/Const.js'
import Utils from '../../Redux/reducers/Utils.js'
import Style from '../Style'


class HomeComponent extends Component {
  constructor(props){
    super(props)
    this.state = {
      lastSteps: 0,
      lastHeartRate: 0,
      stateConnect: false
    }
  }

  shouldComponentUpdate(newProps) {
    if((newProps.state==='CONNECTED' ^ this.state.stateConnect) || (newProps.state==='DISCONNECTED' ^ !this.state.stateConnect)){
      return true
    }
    return false
  }
  componentWillUpdate(newProps){
    if(newProps.state==='CONNECTED'){
      this.setState({
        stateConnect : true
      })
      let serviceUUID = "0000180d-0000-1000-8000-00805f9b34fb"
      let characteristicUUID = "00002a37-0000-1000-8000-00805f9b34fb"
      this.props.selectService(this.props.deviceId, serviceUUID );
      this.props.selectCharacteristic(this.props.deviceId, serviceUUID, characteristicUUID);
      


    }
  }

  render() {
    return (
      <View style={Style.component}>
        <View style={{flexDirection:'row'}}>
          <View style={{flex:1,}}>
            <Text style={{textAlign : 'center'}}>Số bước chân gần nhất</Text>
            <Text style={{textAlign : 'center'}}>12</Text>
          </View>
          <View style={{flex:1,}}>
            <Text style={{textAlign : 'center'}}>Nhịp tim gần nhất</Text>
            <Text style={{textAlign : 'center'}}>80</Text>
          </View>
        </View>
        <View style={{flexDirection:'column'}}>
          <Text style={{textAlign : 'center', alignItems: 'center', height:100}}>Bước chân</Text>
          <Text style={{textAlign : 'center', height: 100}}>Nhịp tim & SpO2</Text>
          <Text style={{textAlign : 'center', height: 100}}>Giấc ngủ (coming soon)</Text>
        </View>
      </View>
    );
  }
}

const mapStateToProps=(state) =>{
  const deviceId = state.getIn(['ble', 'selectedDeviceUUID'])
  const serviceId = state.getIn(['ble', 'selectedServiceUUID'])
  const characteristicId = state.getIn(['ble', 'selectedCharacteristicUUID'])
  return ({
    selectedDeviceId: deviceId,
    state: state.getIn(['ble', 'state']),
    devices: state.getIn(['ble', 'devices']),
    sceneState: state.getIn(['route', 'state']),
    services: state.getIn(['ble', 'devices', deviceId, 'services']),
    characteristic: state.getIn(['ble', 'devices', deviceId, 'services', serviceId, 'characteristics', characteristicId])
  });
}

const mapDispatchToProps = {
  changeDeviceState: ble.changeDeviceState,
  selectService: ble.selectService,
  selectCharacteristic: ble.selectCharacteristic
}

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(HomeComponent)
