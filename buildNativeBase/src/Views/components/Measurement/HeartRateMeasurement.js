import React, { Component } from 'react';
import { Container, Content, Card, CardItem, Text, Body, Icon, Button } from 'native-base';
import ProgressCircle from 'react-native-progress-circle'
import { BarChart, LineChart } from './charts.js'
import styles from './styles';

var idInterval=null
export default class HeartRateMeasurement extends Component {
	// eslint-disable-line
	constructor(props){
		super(props)
		this.state={
			percent: 0
		}
	}
	spin=()=>{
		idInterval=setInterval(()=>{
			this.setState({
				percent:(this.state.percent+10)%101
			})
		},50)
	}
	componentDidMount=()=>{
		// this.spin()
	}
	componentWillUnmount(){
		clearInterval(idInterval)
	}
	render() {
		// eslint-disable-line
		return (
			<Content padder style={{ marginTop: 0,backgroundColor: "#bdc3c7"}}>
				<Card style={styles.card}>
					<ProgressCircle
					  percent={30}
					  radius={130}
					  borderWidth={20}
					  color="#e74c3c"
					  shadowColor="#999"
					  bgColor="#bdc3c7"
					>
					  <Text style={{ fontSize: 60 }}>{'30'}</Text>
					  <Text style={{ fontSize: 20 }}>{'BPM'}</Text>
					</ProgressCircle>
					<LineChart/>
				</Card>
				<Body st>
				 	<Button rounded active bordered danger large
	        	onPress={() => this.props.navigation.navigate("DrawerOpen")}
	        >
	          <Icon name="ios-play"/>
	          <Text>Play</Text>
	          {/*
	          <Icon name="ios-pause" />
	          <Text>Play</Text>
	        	*/}
	        </Button>
        </Body>
			</Content>
		);
	}
}