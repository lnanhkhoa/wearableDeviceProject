// index.js
import React, { Component } from 'react'
import {
  Container,Header, Title, Content, Button, Icon, List, 
  ListItem, Text, Thumbnail, Left, Right, Body, Toast } from "native-base"
import { View, BackHandler, TouchableOpacity} from 'react-native'
import styles from "./styles"
import {connect, ble } from '../../../Redux/'
import { Buffer } from 'buffer'
import { realmMeasureService } from '../../../Realm/'
import { heartRate, deviceUUID } from '../../../Ble/config'

const realmMeasureDataLimit = []
const realmInstanceDataLimit = []
const mount = 0
const isReadable = null
const isNotifying = null
const value = null
const valueAscii = null
const valueHex = null

class Information extends Component{
  constructor(props){
    super(props)
    this.state={
      doubleBackToExitPressedOnce: false
    }
  }
  componentDidMount(){
    BackHandler.addEventListener('hardwareBackPress', this.onBackPress.bind(this));
  }

  componentWillUnmount(){
    BackHandler.removeEventListener('hardwareBackPress', this.onBackPress.bind(this));
  }

  onBackPress(){
    console.log(this.props.dispatch)
    if(this.props.navigation.state.routeName !== 'Information' ) {
      Toast.show({
      text:'Deo phai',
      duration: 1000,
      textStyle:{
        textAlign: 'center'
      }
    });  
      return true;
    }

    if(this.state.doubleBackToExitPressedOnce) {
      BackHandler.exitApp();
    }
    Toast.show({
      text:'Press back again to exit',
      duration: 1000,
      textStyle:{
        textAlign: 'center'
      }
    });
    this.setState({ doubleBackToExitPressedOnce: true });
    setTimeout(() => {
      this.setState({ doubleBackToExitPressedOnce: false });
    }, 2000);
    return true;
  }

  componentWillReceiveProps(newProps){
    realmMeasureDataLimit = realmMeasureService.findWithLimit(50)
    realmInstanceDataLimit= realmMeasureService.findInstanceLimit('Instance_HeartRate', 1)
    mount+=1
  }

  shouldComponentUpdate(){
    return true
  }

  componentWillUpdate(nextProps){
  }

  getRealmInstance(index:number){
    let dataInstanceLimit = realmInstanceDataLimit.slice(index,index+1).map(function(item){
      return item.value
    })
    return dataInstanceLimit
  }

  getRealmMeasure(type: string, index: number){
    let dataLimit = realmMeasureDataLimit.slice(index,index+1).map(function(item){
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

  _onPressStepsCount=()=>{
    this.forceUpdate()
  }

  render() {
    mount +=1
		return (
			<Container style={styles.container}>
        <Header>
          <Left>
            <Button transparent 
            	onPress={() => this.props.navigation.navigate("DrawerOpen")}
            >
              <Icon name="menu" />
            </Button>
          </Left>
          <Body>
            <Title>Information</Title>
          </Body>
          <Right />
        </Header>
        <Content>
        	<View style={{flexDirection:'row'}}>
          <Text>{mount}</Text>
        	<View style={{flex:1,}}>
              <Text style={{textAlign : 'center'}}>Số bước chân gần nhất</Text>
              <Text style={{textAlign : 'center'}}>{this.getRealmMeasure('stepsCount', 0)[0] |0 }</Text>
            </View>
            <View style={{flex:1,}}>
              <Text style={{textAlign : 'center'}}>Nhịp tim gần nhất</Text>
              <Text style={{textAlign : 'center'}}>{this.getRealmInstance(0)[0] |0}</Text>
            </View>
        	</View>
        	<View style={{flexDirection:'column'}}>
            <Text style={{textAlign : 'center', alignItems: 'center',height: 100}}>Bước chân</Text>
            <Text style={{textAlign : 'center', height: 100}}>Nhịp tim & SpO2</Text>            
            <Text style={{textAlign : 'center'}}>{ valueHex }</Text>
            <Text style={{textAlign : 'center', height: 100}}>Giấc ngủ (coming soon)</Text>
          </View>
        </Content>
      </Container>
		);
	}
}

const mapStateToProps=(state) =>{
  return ({
    selectedDeviceUUID: state.getIn(['ble', 'selectedDeviceUUID']),
    state: state.getIn(['ble', 'state']),
    devices: state.getIn(['ble', 'devices']),
    location: state.getIn(['ble', 'devices', deviceUUID, 'services', heartRate.services, 'characteristics', heartRate.location]),
    heartrate: state.getIn(['ble', 'devices', deviceUUID, 'services', heartRate.services, 'characteristics', heartRate.measurement]),

  })
}

const mapDispatchToProps = {
  changeDeviceState: ble.changeDeviceState,
  selectService: ble.selectService,
  selectCharacteristic: ble.selectCharacteristic,
  readCharacteristic: ble.readCharacteristic,
  writeCharacteristic: ble.writeCharacteristic,
  monitorCharacteristic: ble.monitorCharacteristic
}


export default connect(mapStateToProps, mapDispatchToProps)(Information)