import React, { Component } from "react"
import { Image, View, StatusBar, FlatList } from "react-native"
import { Container, Button, H3, Text, Header, Title, Body, Left, Right, Icon, Thumbnail, List, ListItem } from "native-base"
import ProgressCircle from 'react-native-progress-circle'
import styles from "./styles"

import { connect } from 'react-redux'
import * as ble from '../../../Redux/reducers/BleActions'

var datas = [
];

var mount = 0

class ConnectDevice extends Component {
	// eslint-disable-line
  constructor(props){
    super(props)
    this.state={
      refresh: false
    };
  }
  shouldComponentUpdate(nextProps){
    if(nextProps.scanning){
      return true
    }
    return false
  }
  componentWillUpdate(nextProps){
    let devices = nextProps.devices.toObject()
    let keys = Object.keys(devices);
    if(keys.length>0){
      datas = keys.map(function(v){
        return nextProps.devices.getIn([v]).toObject()
      })
    }
  }

  _keyExtractor = (item, index) => item.id;

  scanning=()=>{
    this.props.startScan();
    setTimeout(() =>{
      if(this.props.scanning)
        this.props.stopScan()
    },10000);
  }
 
  refresh=()=>{
    this.setState = {
      refresh: true
    };
    this.scanning()
  }

  componentWillMount=()=>{
    this.scanning()
  }
  componentWillUnmount(){
    this.props.stopScan()
  }
	render() {
		return (
			<Container style={{backgroundColor: "#ecf0f1", flex:1}}>
				<Header style={{backgroundColor: "#2980b9"}}>
          <Left>
            <Button transparent onPress={()=>this.props.navigation.goBack()}>
              <Icon name="arrow-back" />
            </Button>
          </Left>
          <Right>
            <Button transparent onPress={this.props.navigation.navigate("Drawer")}>
              <Icon name="arrow-back" />
            </Button>
          </Right>

          <Body>
            <Title>Connect Your Device</Title>
          </Body>
          <Right />
        </Header>
				<StatusBar barStyle="light-content" />
				<View
					style={{
						flex: 80,
					}}
				>
					<FlatList
            refreshing={this.state.refresh}
            onRefresh={this.refresh}
            keyExtractor={this._keyExtractor}
            onEndReachedThreshold={-0.2}
            onEndReached={() =>{

            }}
            data={datas}
            renderItem={({item}) =>
              <ListItem thumbnail>
                <Left>
                  <ProgressCircle
                    percent={30}
                    radius={30}
                    borderWidth={5}
                    color="#e74c3c"
                    shadowColor="#999"
                  >
                    <Text style={{ fontSize: 10 }}>{'RSSI'}</Text>
                    <Text style={{ fontSize: 20 }}>{'30'}</Text>
                  </ProgressCircle>
                </Left>
                <Body>
                  <Text>{item.name}</Text>
                  <Text numberOfLines={1} note>{item.id}</Text>
                </Body>
                <Right>
                  <Button info onPress={()=>this.props.navigation.navigate("Drawer")}>
                    <Text>Connect</Text>
                  </Button>
                </Right>
              </ListItem>}
          />
				</View>
				<View style={{flex: 20, alignItems: 'center', justifyContent: 'center' }}>
					<Button large  style={{ backgroundColor: "#6FAF98", alignSelf: "center" }}
						onPress={this.refresh}
					>
						<Text>Show vailable Devices</Text>
					</Button>
				</View>
			</Container>
		);
	}
}

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
  }
)
(ConnectDevice);
