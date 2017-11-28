import React, { Component } from 'react';
import {
  StyleSheet,
  View
} from 'react-native';
import Range from './range';

const onSelectRange = function() {
  // body...
}

export default class Switcher extends Component {
  render() {
    const {
      ranges,
      current
    } = this.props;
    return (
      <View style={styles.container}>
        {ranges.map((name, index) =>
          <Range
            name={name}
            active={current == ranges}
            onPress={onSelectRange}
            key={index}
          />)}
      </View>
    );
  }

}

const styles = StyleSheet.create({
  container: {
    flexDirection: 'row',
    justifyContent: 'space-between',
  },
});