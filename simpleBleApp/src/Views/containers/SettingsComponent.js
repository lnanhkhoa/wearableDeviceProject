 // HomeComponent.js
 
import React, { Component } from 'react'
import { View, Text, StyleSheet, FlatList, TouchableOpacity, Alert } from 'react-native'
import { connect } from 'react-redux'
import { Actions } from 'react-native-router-flux'

import ImmutableListView from '../components/assets/ImmutableListView.js'
import * as ble from '../../Redux/reducers/BleActions.js'
import ButtonView from '../components/assets/ButtonView.js'
import * as SceneConst from '../../Redux/reducers/Const.js'
import Style from '../Style'

var dataFlatlistRender = [
  {key: 'My device', title: 'My device', header: true }, 
  {key: 'Device', title: 'Device', iconLeft: '', details:'not connected', expand: 'DEVICES_SCENE', last: true },
  {key: 'Setting goal', title: 'Setting goal', header: true }, 
  {key: 'Sport goal', title: 'Sport goal', iconLeft: '', details:'9000', expand: ''},
  {key: 'Sleep goal', title: 'Sleep goal', iconLeft: '', details:'8.0h', expand: '', last: true },
  {key: 'More', title: 'More', header: true }, 
  {key: 'Unit setting', title: 'Unit setting', iconLeft: '', details:'Metric', expand: ''},
  {key: 'About us', title: 'About us', iconLeft: '', details:'', expand: 'back', last: true }
]

class SettingsComponent extends Component {
  constructor(props){
    super(props);
    this.state={
      dataFlatlistRender: dataFlatlistRender,
      stateConnect: false // true is connected, false is disconnected
    }
  }


  shouldComponentUpdate(newProps, nextState){
    if((newProps.state==='CONNECTED' ^ this.state.stateConnect) || (newProps.state==='DISCONNECTED' ^ !this.state.stateConnect)){
      return true
    }
    return false
  }

  componentWillUpdate(newProps, nextState){
    if(newProps.state==='CONNECTED'){
      this.setState(previousState =>{
        previousState.stateConnect = true
        let filtered=previousState.dataFlatlistRender.filter(item => item.key==='Device' );
        filtered.forEach(item =>{
          item.expand='ADEVICE_SCENE'
          item.details='Connected'
        });
      })
    } else if(newProps.state==='DISCONNECTED') {
      this.setState(previousState =>{
        previousState.stateConnect = false
        let filtered=previousState.dataFlatlistRender.filter(item => item.key==='Device');
        filtered.forEach(item =>{
          item.expand = 'DEVICES_SCENE'
          item.details = 'not connected'
        });
      })
    }
  }

  renderHeader(item){
    return(
      <Text style={styles.header}>
        . {item.title}
      </Text>
    );
  }

  renderChild(item){
    let ite = item;
    _onPressButtonChangeScene=()=>{
      if(ite.expand){
        Actions[ite.expand]();
      }
    }

    return(
      <TouchableOpacity style={styles.child} onPress={_onPressButtonChangeScene}>
        <Text>{item.iconLeft} </Text>
        <Text>{item.title} </Text>
        <Text>{item.details} </Text>
      </TouchableOpacity>
    );
  }

  render() {
    return (
      <View style={Style.component}>
        <FlatList
          data={this.state.dataFlatlistRender}
          extraData={this.state}
          renderItem={({item}) =>{
            if(item.header){
              return(this.renderHeader(item));
            }else{
              return(this.renderChild(item));
            }
            }}
        />
      </View>
    );
  }
}


const styles = StyleSheet.create({
  container:{
    flex: 1,

    backgroundColor: '#fff',
  },
  header:{
    paddingBottom: 10,
    paddingTop: 10,
    paddingLeft: 5
  },
  child:{
    padding:5
  },
  title: {
    fontWeight: 'bold',
    textAlign: 'center',
    padding: 10
  },
  buttonRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
  },
})

export default connect(
  state => {
    const deviceId = state.getIn(['ble', 'selectedDeviceUUID'])
    return({
      devices: state.getIn(['ble', 'devices']),
      scanning: state.getIn(['ble', 'scanning']),
      state: state.getIn(['ble', 'state']),
      selectedDeviceId: state.getIn(['ble', 'selectedDeviceUUID']),
      services: state.getIn(['ble', 'devices', deviceId, 'services'])
    });

  },{
    startScan: ble.startScan,
    stopScan: ble.stopScan,
    changeDeviceState: ble.changeDeviceState
  })
(SettingsComponent)
 