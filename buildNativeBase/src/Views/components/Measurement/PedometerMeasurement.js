import React, { Component } from 'react'
import { TouchableOpacity } from 'react-native'
import { Container, Content, Card, CardItem, Text, Body, Icon, Button } from 'native-base'
import ProgressCircle from 'react-native-progress-circle'
import { BarChart, LineChart } from './charts.js'
import styles from './styles';
import {connect, ble} from '../../../Redux/'
import { Buffer } from 'buffer'
import { customService, deviceUUID } from '../../../Ble/config'
import { realmMeasureService } from '../../../Realm/'


const realmInstanceDataLimit = []
const mount = 0
class PedometerMeasurement extends Component {
  // eslint-disable-line
  constructor(props){
    super(props)
    this.state={
      percent: 0,
      PlayStopButton: true
    }
  }
  componentDidMount(){

  }

  componentWillUnmount(){
  }

  componentWillReceiveProps(newProps){
    realmInstanceDataLimit=realmMeasureService.findInstanceLimit('Instance_StepCount', 50)
  }

  shouldComponentUpdate(newProps){
    return true
  }

  readHeartRateMeasurement(isNotifying: bool){
    this.props.monitorCharacteristic(this.props.selectedDeviceUUID, customService.services, customService.char4, !isNotifying) 
    this.setState({
      PlayStopButton: isNotifying
    })
  }


  getRealmInstance(index:number){
    let dataInstanceLimit = realmInstanceDataLimit.slice(index, index+1).map(function(item){
      return item.value
    })
    return dataInstanceLimit
  }
  getRealmInstanceLineChart(){
    let dataInstanceLimit = realmInstanceDataLimit.map(function(item){
      return item.value
    })
    
    switch(dataInstanceLimit.length){
      case 0: 
        return [0,0]
      case 1:
        dataInstanceLimit.push(0)
      default:
        return dataInstanceLimit 
    }
  }
  update(){
    this.forceUpdate()
  }
  render() {
    // eslint-disable-line
    mount +=1
    const value = this.getRealmInstance(0)[0]
    const arr = this.getRealmInstanceLineChart()
    return (
      <Content padder style={{ marginTop: 0,backgroundColor: "#bdc3c7"}}>
        <TouchableOpacity onPress={this.update.bind(this)}>
          <Text>{mount}</Text>
        </TouchableOpacity>
        <Card style={styles.card}>
          <ProgressCircle
            percent={(value/2) |0}
            radius={130}
            borderWidth={20}
            color="#e74c3c"
            shadowColor="#999"
            bgColor="#bdc3c7"
          >
            <Text style={{ fontSize: 60 }}>{value |0}</Text>
            <Text style={{ fontSize: 20 }}>{'BPM'}</Text>
          </ProgressCircle>
          <LineChart
            data={arr}
          />
        </Card>
        <Body st>
          {this.state.PlayStopButton?
            <Button rounded active bordered danger large
              onPress={() => this.readHeartRateMeasurement(false)}
            >
              <Icon name="ios-play"/>
              <Text>Play</Text>
            </Button>
            :
            <Button rounded active bordered danger large
              onPress={() => this.readHeartRateMeasurement(true)}
            >
              <Icon name="ios-pause" />
              <Text>Stop</Text>
            </Button>
            }
        </Body>
      </Content>
    );
  }
}

const mapStateToProps=(state) =>{
  return ({
    selectedDeviceUUID: state.getIn(['ble', 'selectedDeviceUUID']),
    // location: state.getIn(['ble', 'devices', deviceUUID, 'services', heartRate.services, 'characteristics', heartRate.location]),
    // heartrate: state.getIn(['ble', 'devices', deviceUUID, 'services', heartRate.services, 'characteristics', heartRate.measurement]),
    updateStates: state.getIn(['ble','updateStates'])
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


export default connect( mapStateToProps, mapDispatchToProps)(PedometerMeasurement)