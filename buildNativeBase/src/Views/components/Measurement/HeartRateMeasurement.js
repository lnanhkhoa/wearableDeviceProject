import React, { Component } from 'react'
import { View, TouchableOpacity } from 'react-native'
import { Container, Content, Card, CardItem, Text, Body, Icon, Button } from 'native-base'
import ProgressCircle from 'react-native-progress-circle'
import { BarChart, LineChart } from './charts.js'
import styles from './styles';
import {connect, ble} from '../../../Redux/'
import { Buffer } from 'buffer'
import { heartRate, deviceUUID } from '../../../Ble/config'
import { realmMeasureService } from '../../../Realm/'


const realmInstanceHeartRate = []
const realmInstanceSpO2 = []
const valueHeartRate = 0
const valueSpO2 = 0
const mount = 0
class HeartRateMeasurement extends Component {
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
			realmInstanceHeartRate = realmMeasureService.findInstanceLimit('InstanceHeartRate', 50)
			realmInstanceSpO2 = realmMeasureService.findInstanceLimit('InstanceSpO2', 50)
			let HRobjects = this.getOneObjectRealm('HeartRateMeasurement')
			valueHeartRate = HRobjects.heartRate
			valueSpO2 = HRobjects.spO2
		}

	}

	shouldComponentUpdate(newProps){
		return true
	}

	readHeartRateMeasurement(isNotifying: bool){
    this.props.monitorCharacteristic(this.props.selectedDeviceUUID, heartRate.services, heartRate.measurement, !isNotifying) 
  	this.readHeartRateValue(isNotifying, 500)    
    this.setState({
    	PlayStopButton: isNotifying
    })
  }

  readHeartRateValue(turn:bool, interval: number){
  	if(!turn){
	  	this.state.intervalID = setInterval(()=>{
		    this.props.readCharacteristic(this.props.selectedDeviceUUID, heartRate.services, heartRate.location)
		    this.props.readCharacteristic(this.props.selectedDeviceUUID, heartRate.services, heartRate.measValue)
	  	},interval)
  	}else{
  		clearInterval(this.state.intervalID)
  	}
  }

  getOneObjectRealm(name:string){
  	let objects = realmMeasureService.findWithLimit(name, 1);
  	return objects[0]
  }

  getRealmInstance(name, index:number){
  	let dataInstanceLimit = null
  	switch(name){
  		case 'InstanceHeartRate':
		    dataInstanceLimit = realmInstanceHeartRate.slice(index, index+1).map(function(item){
		      return item.value
		    })
		    break;
		  case 'InstanceSpO2':
		  	dataInstanceLimit = realmInstanceSpO2.slice(index, index+1).map(function(item){
		      return item.value
		    })
		    break;
		  default: dataInstanceLimit = null; break;
		 }
    return dataInstanceLimit
  }

  getRealmInstanceLineChart(name:string){
  	let dataInstanceLimit = null
  	switch(name){
  		case 'InstanceHeartRate':
		  	dataInstanceLimit = realmInstanceHeartRate.map(function(item){
		      return item.value
		    })
		    break;

		  case 'InstanceSpO2':
		    dataInstanceLimit = realmInstanceSpO2.map(function(item){
		      return item.value
		    })
		    break;
		  default: dataInstanceLimit = null; break;	  
  	}
    
  	switch(dataInstanceLimit.length){
  		case 0: 
  			return [0, 0]
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
		// eslint
		// -disable-line
		mount +=1
		const arrayHeartRate = this.getRealmInstanceLineChart('InstanceHeartRate')		
		return (
			<Content padder style={{ marginTop: 0,backgroundColor: "white"}}>
				<Card style={styles.card}>
					<ProgressCircle
					  percent={(valueHeartRate/2.55) |0}
					  radius={100}
					  borderWidth={20}
					  color="#FF307E"
					  shadowColor="#bdc3c7"
					  bgColor="white"
					>
					  <Text style={{ fontSize: 60 }}>{valueHeartRate |0}</Text>
					  <Text style={{ fontSize: 20 }}>{'BPM'}</Text>
					</ProgressCircle>
					<LineChart
						data={arrayHeartRate}
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


export default connect( mapStateToProps, mapDispatchToProps)(HeartRateMeasurement)