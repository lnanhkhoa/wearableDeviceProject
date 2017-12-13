import React, { Component } from "react"
import { Image, View, StatusBar, FlatList, Alert } from "react-native"
import { Container, Content, Button, H3, Text, Header, Title, Body, Left, Right, Icon, Thumbnail, List, ListItem } from "native-base"
import { ProgressDialog } from 'react-native-simple-dialogs';
import ProgressCircle from 'react-native-progress-circle'
import styles from "./styles"
import { connect, ble } from '../../../Redux/'

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
    if(!nextProps.errors.isEmpty()){
      this.setState({
        visibleWaiting: false,
      })
    }
  }

  shouldComponentUpdate(nextProps){    
      return true
  }
  componentWillUpdate(nextProps){    
    if(nextProps.devices !== this.props.devices){
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

  processDeviceConnected(){
    if(this.props.selectedDeviceUUID === "CC:78:AB:AD:D9:84"){
      return true
    }else if(this.props.selectedDeviceUUID !== null ){
      return false

    }
  }
  refresh=()=>{
    this.scanning()
  }

  _keyExtractor = (item, index) => item.id;

  render1() {
    return(
      <Container style={{backgroundColor: "#ecf0f1", flex:1}}>
      </Container>
    );
  }

	render() {
    mount +=1
		return (
			<Container style={{backgroundColor: "#ecf0f1", flex: 1}}>
				<Header style={{backgroundColor: "#2980b9"}}>
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
                      color="#e74c3c" shadowColor="#999"
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
            <Text> {mount} </Text>
  					<Button large  
              style={{ backgroundColor: "#6FAF98", alignSelf: "center" }}
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
  state => ({
    devices: state.getIn(['ble', 'devices']),
    state: state.getIn(['ble', 'state']),
    scanning: state.getIn(['ble', 'scanning']),
    selectedDeviceUUID: state.getIn(['ble','selectedDeviceUUID']),
    errors: state.getIn(['ble','errors'])
  }),
  {
    startScan: ble.startScan,
    stopScan: ble.stopScan,
    changeDeviceState: ble.changeDeviceState,
    pushError: ble.pushError
  }
)
(ConnectDevice);