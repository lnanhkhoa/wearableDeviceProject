import React, { Component } from "react"
import { Image, View, StatusBar, FlatList, Alert } from "react-native"
import { Container, Content, Button, H3, Text, Header, Title, Body, Left, Right, Icon, Thumbnail, List, ListItem } from "native-base"
import { ProgressDialog } from 'react-native-simple-dialogs';
import ProgressCircle from 'react-native-progress-circle'
import styles from "./styles"
import { connect, ble } from '../../../Redux/'
import {devInfo, servicesUUID } from '../../../Ble/config.js'

const datas = [
];
const mount = 0
const idTime = null
const finishProcessConnect = false
const showSkip = true
class ConnectDevice extends Component {
  constructor(props){
    super(props)
    this.state={
      visibleWaiting: false,
    }
    datas = []
  }
  componentWillReceiveProps(nextProps, nextState){
    if(!finishProcessConnect && nextProps.state === 'CONNECTED'){
      this.setState({
        visibleWaiting: false,
      })
    }
    if(!nextProps.errors.isEmpty() && this.state.visibleWaiting){
      Alert.alert(
        'Message',
        "Error",
        [
          {text: 'OK'},
        ],
        { cancelable: false }
      )
      this.setState({
        visibleWaiting: false,
      })
    }
  }

  shouldComponentUpdate(nextProps){    
      return true
  }
  componentWillUpdate(nextProps){    
    if(nextProps.devices !== this.props.devices && !!nextProps.devices){
      let devices = nextProps.devices.toObject()
      let keys = Object.keys(devices);
      if(keys.length>0){
        datas = keys.map(function(v){
          return nextProps.devices.getIn([v]).toObject()
        })
      }
    }
  }

  componentDidUpdate=(preProps)=>{
    if(!finishProcessConnect && preProps.state === 'CONNECTED'){
      finishProcessConnect = true
      if(this.processDeviceConnected()){
        showSkip=false
        this.props.navigation.navigate("Drawer")
      }else{
        Alert.alert(
          'Message',
          "Can't connect to this device!",
          [
            {text: 'OK'},
          ],
          { cancelable: false }
        )
        this.props.changeDeviceState(this.props.selectedDeviceUUID, ble.DEVICE_STATE_DISCONNECT);
      }
    }
  }

  componentDidMount=()=>{
    this.scanning()
  }
  componentWillUnmount(){
    mount = 0
    if(this.props.scanning){
      this.props.stopScan()
    }
    this.setState({
      visibleWaiting: false,
    })
  }
  
  scanning=()=>{
    if(this.props.scanning){
      this.props.stopScan()
    }
    this.props.startScan();
    setTimeout(()=>{
      if(this.props.scanning)
        this.props.stopScan()
    },10000);
  }

  connectingDevice=(id, name)=>{
    if(name !== null){
      this.setState({
        visibleWaiting: true
      })
      this.props.stopScan()
      this.props.changeDeviceState(id, ble.DEVICE_STATE_CONNECT);
      finishProcessConnect = false
    }else{
      Alert.alert(
        'Message',
        'Device should have name!',
        [
          {text: 'OK'},
        ],
        { cancelable: false }
      )
    }
  }

  checkInArray(text, arrSource){
      for (let i = 0; i < arrSource.length; i++) {
        if (arrSource[i].indexOf(text) !== -1) return true
      }
      return false
  }

  processDeviceConnected=()=>{
    if(!!this.props.checkDevice){
      let servicesKeys = this.props.checkDevice.keySeq().toArray()
      let servicesKeysNeedCheck = servicesUUID.keys
      for (let i = 0; i < servicesKeysNeedCheck.length; i++) {
        if(!this.checkInArray(servicesKeysNeedCheck[i], servicesKeys)){ 
          return false
        } 
      }
      return true
    }
  }
  refresh=()=>{
    this.scanning()
  }

  _keyExtractor = (item, index) => item.id;

	render() {
    mount +=1
		return (
			<Container style={{backgroundColor: "white", flex: 1}}>
				<Header style={{backgroundColor: "#2980b9"}} androidStatusBarColor='#2980b9'>
          <Left>
            <Button transparent onPress={()=>this.props.navigation.goBack()}>
              <Icon name="arrow-back" />
            </Button>
          </Left>

          <Body>
            <Title>Connect Your Device</Title>
          </Body>
          {showSkip && <Right>
            <Button transparent onPress={()=>{
              showSkip=false
              this.props.navigation.navigate("Drawer")
            }}>
              <Text>SKIP</Text>
            </Button>
          </Right>}
        </Header>
				<StatusBar barStyle="light-content" />
        <ProgressDialog 
          visible={this.state.visibleWaiting} 
          message="Please, wait..."
        />
        <View style={{flex:1 }}>
  				<View style={{ flex: 80, }} >
  					<FlatList
              refreshing={false}
              onRefresh={this.refresh}
              keyExtractor={this._keyExtractor}
              onEndReachedThreshold={-0.2}
              onEndReached={() =>{}}
              data={datas}
              renderItem={({item, index}) =>
                <ListItem thumbnail>
                  <Left>
                    <ProgressCircle
                      percent={Math.abs(item.rssi)} radius={30} borderWidth={5}
                      color="#c0392b" shadowColor="#bdc3c7"
                    >
                      <Text style={{ fontSize: 10 }}>{'RSSI'}</Text>
                      <Text style={{ fontSize: 20 }}>{item.rssi}</Text>
                    </ProgressCircle>
                  </Left>
                  <Body>
                    <Text>{item.name}</Text>
                    <Text numberOfLines={1} note>{item.id}</Text>
                  </Body>
                  <Right>
                    <Button info
                      disabled={this.state.disabledButton}
                      onPress={()=>this.connectingDevice(item.id, item.name)} >
                      <Text>Connect</Text>
                    </Button>
                  </Right>
                </ListItem>
              }
            />
  				</View>
  				<View style={{flex: 20, alignItems: 'center', justifyContent: 'center' }}>
  					<Button large  
              style={{ backgroundColor: "#2E7D32", alignSelf: "center" }}
  						onPress={this.refresh} >
  						<Text>Show vailable Devices</Text>
  					</Button>
  				</View>
        </View>
			</Container>
		);
	}
}

export default connect(
  state => {
    const selectedDeviceUUID = state.getIn(['ble','selectedDeviceUUID'])
    return {
      devices: state.getIn(['ble', 'devices']),
      state: state.getIn(['ble', 'state']),
      scanning: state.getIn(['ble', 'scanning']),
      selectedDeviceUUID: selectedDeviceUUID,
      errors: state.getIn(['ble','errors']),
      checkDevice: state.getIn(['ble', 'devices', selectedDeviceUUID, 'services'])    
    }
  },{
    startScan: ble.startScan,
    stopScan: ble.stopScan,
    changeDeviceState: ble.changeDeviceState,
    pushError: ble.pushError
  }
)
(ConnectDevice);