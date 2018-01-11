/*global setInterval*/
/* demo.js is loaded by both index.ios.js and index.android.js */

import { random, range } from "lodash";
import React, { Component } from "react";
import {
  ScrollView,
  StyleSheet,
  Platform,
  Text,
  View,
  Dimensions
} from "react-native";
import Svg from "react-native-svg";
import {
  VictoryChart,
  VictoryStack,
  VictoryArea,
  VictoryTheme,
  VictoryLine,
  VictoryBar,
  VictoryAxis,
  VictoryContainer
} from "victory-native";

export class AreaChart extends Component {
  constructor(props) {
    super(props);
  }
  render() {
    return (
        <View>
          <VictoryArea
            style={{
              data: { fill: "#E5191C" },
              parent: { border: "1px solid #ccf"}
            }}
            standalone={true}
            padding={{left:10}}
            // {/*interpolation="natural" */}
            data={this.props.data}
          />
          </View>
    );
  }
}


export class LineChart extends Component {
  constructor(props) {
    super(props);
    this.state={
      scrollEnabled: true
    }
  }
  
  render() {
    return (
      <ScrollView scrollEnabled={this.state.scrollEnabled}>
        <VictoryChart
          containerComponent={
            <VictoryContainer
              onTouchStart={() => this.setState({ scrollEnabled: false })}
              onTouchEnd={() => this.setState({ scrollEnabled: true })}
            />
          }
          padding={{left: 10, right:2, top:20, bottom:50}}
        >
          <VictoryLine data={this.props.data} style={{ data: {stroke: "#439CBB", strokeWidth: 4}}}
          />
          <VictoryLine data={this.props.data1} style={{ data: {stroke: "#DF1F5A", strokeWidth: 4}}}
          />
          <VictoryAxis
            style={
              { axis: {stroke: "none"} }
            } 
          />
        </VictoryChart>
      </ScrollView>

    );
  }
}

export class BarChart extends Component {
  constructor(props) {
    super(props);
    this.state={
      scrollEnabled: true
    }
  }
  render() {
    return (
      <ScrollView scrollEnabled={this.state.scrollEnabled}>
        <VictoryChart
          containerComponent={
            <VictoryContainer
              onTouchStart={() => this.setState({ scrollEnabled: false })}
              onTouchEnd={() => this.setState({ scrollEnabled: true })}
            />
          }
          // padding={{top:20, bottom:50}}
        >
          <VictoryBar data={this.props.data} />
          <VictoryAxis
            // style={
            //   { axis: {stroke: "none"} }
            // } 
          />
        </VictoryChart>
      </ScrollView>

    );
  }
}

export class ContainerChart extends Component {
  constructor(props){
    super(props)
    this.state={
      scrollEnabled: true
    }
  }
  render(){
    return (
      <ScrollView scrollEnabled={this.state.scrollEnabled}>
        <VictoryChart
          containerComponent={
            <VictoryContainer
              onTouchStart={() => this.setState({ scrollEnabled: true })}
              onTouchEnd={() => this.setState({ scrollEnabled: true })}
            />
          }
        >
        <VictoryBar/>
        </VictoryChart>
        <VictoryChart
          containerComponent={
            <VictoryContainer
              onTouchStart={() => this.setState({ scrollEnabled: true })}
              onTouchEnd={() => this.setState({ scrollEnabled: true })}
            />
          }
        >
        <VictoryBar/>
        </VictoryChart>
        <VictoryChart
          containerComponent={
            <VictoryContainer
              onTouchStart={() => this.setState({ scrollEnabled: true })}
              onTouchEnd={() => this.setState({ scrollEnabled: true })}
            />
          }
        >
        <VictoryBar/>
        </VictoryChart>
      </ScrollView>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    paddingLeft: 0,
    paddingRight: 0,
    paddingTop: 10
  },
  text: {
    fontSize: 18,
    fontFamily: (Platform.OS === "ios") ? "Menlo" : "monospace",
    fontWeight: "bold",
    marginTop: 25,
    marginBottom: 20
  },
  heading: {
    fontSize: 27,
    fontFamily: (Platform.OS === "ios") ? "Menlo" : "monospace",
    fontWeight: "bold",
    marginTop: 30,
    marginBottom: 30
  }
});
