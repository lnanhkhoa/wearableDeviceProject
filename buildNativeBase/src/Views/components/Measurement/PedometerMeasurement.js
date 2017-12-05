import React, { Component } from 'react';
import { Container, Content, Card, CardItem, Text, Body, Icon, Button } from 'native-base';
import ProgressCircle from 'react-native-progress-circle'
import { LineChart } from './charts.js'
import styles from './styles';

export default class PedometerMeasurement extends Component {
	// eslint-disable-line
  constructor(props){
    super(props)
    this.state={
      button: false
    }
  }
	render() {
		// eslint-disable-line
		return (
		  <Content padder style={{ marginTop: 0,backgroundColor: "#bdc3c7"}}>
        <Card style={styles.card}>
            <ProgressCircle
              percent={30}
              radius={150}
              borderWidth={20}
              color="#e74c3c"
              shadowColor="#999"
              bgColor="#bdc3c7"
            >
              <Text style={{ fontSize: 60 }}>{'30'}</Text>
              <Text style={{ fontSize: 20 }}>{'Steps'}</Text>
            </ProgressCircle>
        </Card>
        <Body>
          {this.state.button?
            <Button rounded active bordered danger large
            >
              <Icon name="ios-play"/>
              <Text>Play</Text>
            </Button>
            :
            <Button rounded active bordered danger large
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
