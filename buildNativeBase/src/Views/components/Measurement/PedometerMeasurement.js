import React, { Component } from 'react'
import { TouchableOpacity } from 'react-native'
import { Container, Content, Card, CardItem, Text, Body, Icon, Button } from 'native-base'
import ProgressCircle from 'react-native-progress-circle'
import { BarChart, LineChart } from './charts.js'
import styles from './styles';
import {connect, ble} from '../../../Redux/'
import { Buffer } from 'buffer'
import { pedometerService, deviceUUID } from '../../../Ble/config'
import { realmMeasureService } from '../../../Realm/'

const realmPedometerDataLimit = []
const mount = 0
const target = 100
const targetPercent = 100/(target)

class PedometerMeasurement extends Component {
  // eslint-disable-line
  constructor(props){
    super(props)
    this.state={
      percent: 0,
      PlayStopButton: true,
      intervalID: null
    }
  }

  componentDidMount(){

  }

  componentWillUnmount(){

  }

  componentWillReceiveProps(newProps){
    if(newProps.updateStates !== this.props.updateStates){
      realmPedometerDataLimit=realmMeasureService.findInstanceLimit('PedometerMeasurement', 10)
      console.log('has update')
    } 
  }

  shouldComponentUpdate(newProps){
    return true
  }  


  readPedometerMeasurement(turn:bool){
    if(turn){
      this.state.intervalID = setInterval(()=>{
        this.props.readCharacteristic(this.props.selectedDeviceUUID, pedometerService.services, pedometerService.measurement)

      },1000)
    }else{
      clearInterval(this.state.intervalID)
    }
    this.setState({ PlayStopButton: !turn })
  }

  getPedometerValue(index:number){
    let dataPedometerLimit = realmPedometerDataLimit.slice(index, index+1).map(function(item){
      return item.stepsCount
    })
    return dataPedometerLimit
  }

  getRealmInstanceBarChart(){
    let dataPedometerLimit = realmPedometerDataLimit.map(function(item){
      return item.stepsCount
    })
    
    switch(dataPedometerLimit.length){
      case 0: 
        return [0,0]
      case 1:
        dataPedometerLimit.push(0)
      default:
        return dataPedometerLimit 
    }
  }
  update(){
    this.forceUpdate()
  }
  render() {
    // eslint-disable-line
    mount +=1
    const value = this.getPedometerValue(0)[0]

    return (
      <Content padder style={{ marginTop: 0,backgroundColor: "white"}}>
        <TouchableOpacity onPress={this.update.bind(this)}>
        </TouchableOpacity>
        <Card style={styles.card}>
          <ProgressCircle
            percent={(value/targetPercent) |0}
            radius={100}
            borderWidth={20}
            color="#3498db"
            shadowColor="#bdc3c7"
            bgColor="white"
          >
            <Text style={{ fontSize: 60 }}>{value |0}</Text>
            <Text style={{ fontSize: 20 }}>{'steps'}</Text>
          </ProgressCircle>
        </Card>
        <Body st>
          {this.state.PlayStopButton?
            <Button rounded active bordered danger large
              onPress={() => this.readPedometerMeasurement(true)}
            >
              <Icon name="ios-play"/>
              <Text>Play</Text>
            </Button>
            :
            <Button rounded active bordered danger large
              onPress={() => this.readPedometerMeasurement(false)}
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