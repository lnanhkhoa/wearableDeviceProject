// index.js
import React, { Component } from 'react'
import { View, StyleSheet } from 'react-native'
import {Icon, Container, Header, Left, Body, Right, Button, Title, Content, Tabs, Tab } from 'native-base'
import TabOne from './PedometerMeasurement.js';
import TabTwo from './HeartRateMeasurement.js';
import styles from './styles'

export default class Measurement extends Component {
  render(){ 
    return(
      <Container style={styles.container}>
        <Header hasTabs>
          <Left>
            <Button 
              transparent 
              onPress={() => this.props.navigation.navigate("DrawerOpen")}
            >
              <Icon name="menu" />
            </Button>
          </Left>
          <Body>
            <Title>Measurement</Title>
          </Body>
          <Right />
        </Header>
        <Tabs tabBarPosition="bottom">
          <Tab heading="Heart Rate">
            <TabTwo />
          </Tab>
          <Tab heading="Steps Count">
            <TabOne />
          </Tab>
        </Tabs>
      </Container>
    )
  }
}