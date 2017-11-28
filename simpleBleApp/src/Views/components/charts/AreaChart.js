/*global setInterval*/
/* demo.js is loaded by both index.ios.js and index.android.js */

import { random, range } from "lodash";
import React, { Component } from "react";
import {
  ScrollView,
  StyleSheet,
  Platform,
  Text
} from "react-native";
import Svg from "react-native-svg";
import {
  VictoryChart,
  VictoryStack,
  VictoryArea,
  VictoryTheme
} from "victory-native";

export default class AreaChart extends Component {
  constructor(props) {
    super(props);
    this.state = {
      scrollEnabled: true
    };
  }
  render() {
    return (
      // <ScrollView contentContainerStyle={styles.container} scrollEnabled={this.state.scrollEnabled}>
        <VictoryChart
        >
          <VictoryArea
            style={{
              data: { fill: "#c43a31" },
              parent: { border: "1px solid #ccf"}
            }}
            standalone={true}
            animate={{
              duration: 2000,
              onLoad: { duration: 1000 }
            }}
            data={[
              { x: 1, y: 2 },
              { x: 2, y: 3 },
              { x: 3, y: 5 },
              { x: 4, y: 4 },
              { x: 5, y: 7 },
              { x: 6, y: 6 }
            ]}
          />
        </VictoryChart>
      // </ScrollView>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    alignItems: "center",
    backgroundColor: "#e1d7cd",
    justifyContent: "center",
    paddingLeft: 50,
    paddingRight: 50,
    paddingTop: 50
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