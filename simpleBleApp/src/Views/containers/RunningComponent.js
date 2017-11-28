'use strict';
import React, { Component } from 'react';
import { connect } from 'react-redux';
import { View, Switch, Text, TextInput, StyleSheet } from 'react-native';
import { Buffer } from 'buffer';

import * as ble from '../../Redux/reducers/BleActions.js';
import ButtonView from '../components/assets/ButtonView.js';
import Style from '../Style.js';

class RunningComponent extends Component {

  constructor(props) {
    super(props);
    this.state = { 
    	newValue: '',
    	stateConnect : false
    }
  }

  // shouldComponentUpdate(newProps) {
  //   if((newProps.state==='CONNECTED' ^ this.state.stateConnect) || (newProps.state==='DISCONNECTED' ^ !this.state.stateConnect)){
  //     return true
  //   }
  //   return false
  // }
  componentWillMount(){
    if(this.props.state==='CONNECTED'){
      this.setState({
        stateConnect : true
      })
      console.log('here')
      // let serviceUUID = "0000180d-0000-1000-8000-00805f9b34fb"
      // let characteristicUUID = "00002a38-0000-1000-8000-00805f9b34fb"
      // this.props.selectService(this.props.selectedDeviceUUID, serviceUUID );
      // this.props.selectCharacteristic(this.props.selectedDeviceUUID, serviceUUID, characteristicUUID);
    }
  }

  render() {
    const characteristic = this.props.characteristic;
    const uuid = characteristic.get('uuid')
    const isReadable = characteristic.get('isReadable')
    const isWritable = characteristic.get('isWritable')
    const isNotifiable = characteristic.get('isNotifiable')
    const isNotifying = characteristic.get('isNotifying')
    var value = characteristic.get('value')
    var valueAscii = '';
    var valueHex = '';

    if (value) {
      valueAscii = new Buffer(value.toString(), "base64").toString('ascii')
      valueHex = new Buffer(value.toString(), "base64").toString('hex')
    } else {
      valueAscii = '-'
      value = '-'
    }
    console.log('1')
    const read = () => {
      this.props.readCharacteristic(this.props.deviceId,
                                    this.props.serviceId,
                                    this.props.characteristicId)
    }

    const write = () => {
      this.props.writeCharacteristic(this.props.deviceId,
                                    this.props.serviceId,
                                    this.props.characteristicId,
                                    new Buffer(this.state.newValue, 'hex').toString('base64'))
    }

    const notify = () => {
      this.props.monitorCharacteristic(this.props.deviceId,
                                       this.props.serviceId,
                                       this.props.characteristicId,
                                       !isNotifying)
    }

    return (
      <View style={[Style.component, {flex: 0}]}>
        <Text style={styles.header}>UUID</Text>
        <Text style={{fontSize: 12}}>{uuid}</Text>
        
        <View style={{ flexDirection: 'row', justifyContent: 'space-between' }}>
          <View>
            <Text style={styles.header}>isReadable: </Text>
            <Text>{isReadable ? 'true' : 'false'}</Text>
          </View>
          <View>
            <Text style={styles.header}>isWritable: </Text>
            <Text>{isWritable ? 'true' : 'false'}</Text>
          </View>
          <View>
            <Text style={styles.header}>isNotifiable: </Text>
            <Text>{isNotifiable ? 'true' : 'false'}</Text>
          </View>
        </View>

        <Text style={styles.header}>Notify:</Text>
        <Switch 
          disabled={!isNotifiable}
          value={isNotifying} 
          onValueChange={notify}
        />
        
        <Text style={styles.header}>Value HEX:</Text>
        <View style={{ flexDirection: 'row'}}>
          <Text style={{flex:1}}>{valueHex}</Text>
          <ButtonView style={{flex:1}} onClick={read} disabled={!isReadable} color={'#beffc6'} text={'Read'}/>
        </View>

        <Text style={styles.header}>Value ASCII:</Text>
        <Text>{valueAscii}</Text>

        <Text style={styles.header}>New value:</Text>
        <View style={{ flex: 1, flexDirection: 'row'}}>
          <TextInput 
            style={{ height: 40, flex: 1}} 
            value={this.state.newValue}
            autoCorrect={false}
            onChangeText={(text) => this.setState({newValue: text})}
          />
          <ButtonView onClick={write} disabled={!isWritable} color={'#ffd0d3'} text={'Write'}/>
        </View>
      </View>
    )
  }
}

const styles = StyleSheet.create({
  header: {
    paddingTop: 20,
    paddingBottom: 5,
    fontSize: 10,
  }
});

const mapStateToProps=(state) => {
  const deviceId = state.getIn(['ble', 'selectedDeviceUUID'])
  const serviceId = state.getIn(['ble', 'selectedServiceUUID'])
  const characteristicId = state.getIn(['ble', 'selectedCharacteristicUUID'])
  return ({
  	deviceId,
  	serviceId,
  	characteristicId,
    state: state.getIn(['ble', 'state']),
    characteristic: state.getIn(['ble', 'devices', deviceId, 'services', serviceId, 'characteristics', characteristicId])
  });
};

const mapDispatchToProps={
  readCharacteristic: ble.readCharacteristic,
  writeCharacteristic: ble.writeCharacteristic,
  monitorCharacteristic: ble.monitorCharacteristic,
  selectService: ble.selectService,
  selectCharacteristic: ble.selectCharacteristic
};

export default connect(mapStateToProps,mapDispatchToProps)(RunningComponent)