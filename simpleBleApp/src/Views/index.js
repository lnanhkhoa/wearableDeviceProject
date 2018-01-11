 import React, { Component } from 'react'
 import {Platform, View, Text, StyleSheet} from 'react-native'
import { Router, Scene, Tabs, Stack, Actions } from 'react-native-router-flux'
import { connect } from 'react-redux'
import * as SceneConst from '../Redux/reducers/Const'
import * as ble from '../Redux/reducers/BleActions'

import HomeComponent from './containers/Home/home.js'
import ProfileComponent from './containers/Profiles/profiles.js'
import SettingsComponent from './containers/Settings/settings.js'
import DeviceConnectionComponent from './containers/Settings/devicesConnection.js'
import ADeviceComponent from './containers/Settings/aDevice.js'
import RunningComponent from './containers/Running/RunningComponent.js'

// import ScannedDevicesComponent from './containers/scanning/ScannedDevicesComponent.js'
// import ServicesComponent from './containers/services/ServicesComponent.js'
// import CharacteristicsComponent from './containers/characteristics/CharacteristicsComponent.js'
// import CharacteristicDetailsComponent from './containers/characteristics/CharacteristicDetailsComponent.js'


const TabIcon = ({ selected, title }) => {
  return (
    <Text style={{color: selected ? 'red' :'black'}}>{title}</Text>
  );
}

class RootComponent extends Component {
	constructor(props){
    super(props);
    this.state={
    }
  }

  _onBackScannedDevice=()=>{
    this.props.stopScan();
    Actions.pop();
  }

  render() {
 		return (
 			<Router>
      <Scene key="root"> 
        <Scene tabs key="tabbar" tabBarStyle={styles.tabBarStyle} 
          activeBackgroundColor="white" inactiveBackgroundColor="rgba(255, 0, 0, 0.5)" >
          <Scene key={SceneConst.HOME_SCENE} title="Infomation" icon={TabIcon}>
            <Scene key="info" component={HomeComponent} title="Information" />
          {/* <Scene key="info1" component={SettingsComponent} title="Setting" /> */}
          </Scene>
          {/*<Scene key={SceneConst.PROFILE_SCENE} title="Profile" icon={TabIcon}>
            <Scene key="profile" component={ProfileComponent} title="Profile" />
          </Scene>
          <Scene key={SceneConst.SERVICES_SCENE} title="Running" icon={TabIcon}>
            <Scene key="profile" component={RunningComponent} title="Running"  />
          </Scene>
        */}
          <Scene key={SceneConst.SETTINGS_SCENE} title="Setting" icon={TabIcon}>
            <Scene key="settings1" component={SettingsComponent} title="Settings"  />
          </Scene>
        </Scene>
        {/*
        <Scene key={SceneConst.DEVICES_SCENE} title="Device Connection" component={HomeComponent} onBack={this._onBackScannedDevice} />
        <Scene key={SceneConst.ADEVICE_SCENE} title="A Device" component={HomeComponent} />
        <Scene key={SceneConst.CHARACTERISTICS_SCENE} component={HomeComponent} title="Running" />
        */}
      </Scene>
    </Router>
 		);
 	}
}

const mapStateToProps = (state) => ({
  selectedDeviceId: state.getIn(['ble', 'selectedDeviceUUID']),
  sceneState: state.getIn(['route', 'state'])
});

const mapDispatchToProps = {
  stopScan: ble.stopScan,
	changeDeviceState: ble.changeDeviceState
}
export default connect( mapStateToProps, mapDispatchToProps)(RootComponent)

const styles = StyleSheet.create({
  container: {
    flex: 1, backgroundColor: 'transparent', justifyContent: 'center',
    alignItems: 'center',
  },
  tabBarStyle: {
    backgroundColor: '#eee',
  },
  tabBarSelectedItemStyle: {
    backgroundColor: '#ddd',
  },
});
