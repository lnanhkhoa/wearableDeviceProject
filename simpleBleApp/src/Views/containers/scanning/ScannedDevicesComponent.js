'use strict';

import React, { Component } from 'react'
import { View, StyleSheet } from 'react-native'
import { connect } from 'react-redux'
import { Actions } from 'react-native-router-flux'

import ButtonView from '../assets/ButtonView.js'
import ImmutableListView from '../assets/ImmutableListView.js'
import ScannedDeviceView from './ScannedDeviceView.js'
import * as ble from '../../../Redux/reducers/BleActions.js'
import * as SceneConst from '../../../Redux/reducers/Const.js'
import Style from '../../Style.js'

class ScannedDevicesComponent extends Component {
  
  componentWillMount(){
    this.props.startScan();
    setTimeout(() =>{
      if(this.props.scanning)
        this.props.stopScan()
    },10000);
  }

  _renderScannedDeviceCell(rowData) {
    const connectToDevice = () => {
      this.props.stopScan();
      this.props.changeDeviceState(rowData.get('id'), ble.DEVICE_STATE_CONNECT);
      Actions.pop();
    }
    
    return (
      <ScannedDeviceView
        name={rowData.get('name')}
        id={rowData.get('id')}
        rssi={rowData.get('rssi')}
        onClick={connectToDevice}
      />
    )
  }

  render() {
    return (
      <View style={Style.component}>
        <ImmutableListView 
          startScan={this.props.startScan} 
          stopScan={this.props.stopScan}
          data={this.props.devices}
          onRenderCell={this._renderScannedDeviceCell.bind(this)}
        />
      </View>
    )
  }
}

var styles = StyleSheet.create({
  title: {
    fontWeight: 'bold',
    textAlign: 'center',
    padding: 10
  },
  buttonRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
  },
});

export default connect(
  state => ({
    devices: state.getIn(['ble', 'devices']),
    state: state.getIn(['ble', 'state']),
    scanning: state.getIn(['ble', 'scanning'])
  }),
  {
    startScan: ble.startScan,
    stopScan: ble.stopScan,
    changeDeviceState: ble.changeDeviceState
  })
(ScannedDevicesComponent)
