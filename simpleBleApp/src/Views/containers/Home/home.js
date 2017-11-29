 // HomeComponent.js
 
import React, { Component } from 'react'
import { View, Text, StyleSheet, ScrollView, TouchableOpacity } from 'react-native'
import { connect } from 'react-redux'
import { Actions } from 'react-native-router-flux'

import * as ble from '../../../Redux/reducers/BleActions'
import * as SceneConst from '../../../Redux/reducers/Const.js'

import { AreaChart, LineChart, BarChart } from '../../components/charts/charts.js'
import realmMeasureService from '../../../Realm/realmControl.js'
import Utils from '../../../Redux/reducers/Utils.js'
import Style from '../../Style'

class HomeComponent extends Component {
  constructor(props){
    super(props)
    this.state = {
      lastSteps: 0,
      lastHeartRate: 0,
      stateConnect: false,
      scrollEnabled: true
    }
  }

  getRealmMeasure(type, limit){
    let realmMeasureDataLimit = realmMeasureService.findWithLimit(limit);
    let dataLimit = realmMeasureDataLimit.map(function(item, index){
      switch(type){
        case 'stepsCount':
          return( item.stepsCount );
        case 'distance':
          return( item.distance );
        case 'caloriesBurn':
          return( item.caloriesBurn );
        case 'heartrate':
          return( item.heartRate );
        case 'spO2':
          return( item.spO2 );
        case 'bloodPressure':
          return( item.bloodPressure );
      }
    })
    return dataLimit
  }

  componentWillReceiveProps(newProps){
    this.setState({
    })
  }

  // shouldComponentUpdate(newProps) {
    // if((newProps.state==='CONNECTED' ^ this.state.stateConnect) || (newProps.state==='DISCONNECTED' ^ !this.state.stateConnect)){
    // if(false){
      // return true
    // }
    // return false
  // }
  componentWillUpdate(newProps){

    // if(newProps.state==='CONNECTED'){
    //   this.setState((prevState) =>{
    //     prevState.stateConnect = true,
    //     prevState.dataHeartRateLimit.unshift(Math.random()*100|0)
    //     prevState.dataHeartRateLimit.pop()
    //     return (prevState);
    //   })
      // let serviceUUID = "0000180d-0000-1000-8000-00805f9b34fb"
      // let characteristicUUID = "00002a37-0000-1000-8000-00805f9b34fb"
      // this.props.selectService(this.props.deviceId, serviceUUID );
      // this.props.selectCharacteristic(this.props.deviceId, serviceUUID, characteristicUUID);

    // }
  }

  _onPressStepsCount=() =>{

  }
  componentDidMount=()=>{
    this.setState({
      lastSteps: this.getRealmMeasure('stepsCount',1)[0],
      lastHeartRate: this.getRealmMeasure('heartrate',1)[0]
    })
  }

  render() {
    return (
      <View style={Style.component}>
        <ScrollView scrollEnabled={true}>
          <View style={{flexDirection:'row'}}>
            <View style={{flex:1,}}>
              <Text style={{textAlign : 'center'}}>Số bước chân gần nhất</Text>
              <Text style={{textAlign : 'center'}}>{this.state.lastSteps}</Text>
            </View>
            <View style={{flex:1,}}>
              <Text style={{textAlign : 'center'}}>Nhịp tim gần nhất</Text>
              <Text style={{textAlign : 'center'}}>{this.state.lastHeartRate}</Text>
            </View>
          </View>
          <View style={{flexDirection:'column'}}>
            <TouchableOpacity onPress={this._onPressStepsCount}>
              <Text style={{textAlign : 'center', alignItems: 'center',}}>Bước chân</Text>
            </TouchableOpacity>
            <BarChart data={this.getRealmMeasure('stepsCount', 20)}/>
            <Text style={{textAlign : 'center', height: 100}}>Nhịp tim & SpO2</Text>
            <LineChart data={this.getRealmMeasure('heartrate', 50)} data1={this.getRealmMeasure('spO2',50)} />
            <Text style={{textAlign : 'center', height: 100}}>Giấc ngủ (coming soon)</Text>
            <LineChart data={this.getRealmMeasure('bloodPressure', 50)} data1={this.getRealmMeasure('distance', 50)}/>
          </View>
        </ScrollView>
      </View>
    );
  }
}

const mapStateToProps=(state) =>{
  // const deviceId = state.getIn(['ble', 'selectedDeviceUUID'])
  // const serviceId = state.getIn(['ble', 'selectedServiceUUID'])
  // const characteristicId = state.getIn(['ble', 'selectedCharacteristicUUID'])
  return ({
  //   selectedDeviceId: deviceId,
  //   state: state.getIn(['ble', 'state']),
  //   devices: state.getIn(['ble', 'devices']),
  //   sceneState: state.getIn(['route', 'state']),
  //   services: state.getIn(['ble', 'devices', deviceId, 'services']),
  //   characteristic: state.getIn(['ble', 'devices', deviceId, 'services', serviceId, 'characteristics', characteristicId])
  });
}

const mapDispatchToProps = {
  // changeDeviceState: ble.changeDeviceState,
  // selectService: ble.selectService,
  // selectCharacteristic: ble.selectCharacteristic
}

export default connect(
  mapStateToProps,
  mapDispatchToProps
)(HomeComponent)