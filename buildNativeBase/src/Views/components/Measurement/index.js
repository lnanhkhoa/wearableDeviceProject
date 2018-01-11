// index.js
import React, { Component } from 'react'
import { View, StyleSheet, Platform } from 'react-native'
import {Icon, Container, StatusBar, ScrollableTab, Header, Left, Body, Right, Button, Title, Content, Tabs, Tab } from 'native-base'
import TabOne from './HeartRateMeasurement.js';
import TabTwo from './PedometerMeasurement.js';
import styles, {primaryColor} from './styles'

export default class Measurement extends Component {
  render(){ 
    return(
      <Container style={styles.container}>
        <Header hasTabs 
          style={styles.header}
          androidStatusBarColor={primaryColor}
        >
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
        <Tabs 
          tabBarPosition="bottom"
          tabBarUnderlineStyle={styles.tabBarUnderlineStyle}
          >
          <Tab heading="Heart Rate"
            tabStyle = {{backgroundColor: primaryColor}}
            activeTabStyle = {{backgroundColor: primaryColor}}
            // textStyle = {}
          >
            <TabOne />
          </Tab>
          <Tab heading="Steps Count"
            tabStyle = {{backgroundColor: primaryColor}}
            activeTabStyle = {{backgroundColor: primaryColor}}
          >
            <TabTwo />
          </Tab>
        </Tabs>
      </Container>
    )
  }
}