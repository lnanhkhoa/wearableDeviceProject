'use strict';

import React, { Component } from 'react';
import { View, AppRegistry } from 'react-native';
import { Iterable } from 'immutable';
import { Provider } from 'react-redux';
import { createLogger } from 'redux-logger';
import configureStore from './src/Redux/store/configureStore'

import BleComponent from './src/Ble/BleComponent'
import ErrorComponent from './src/Views/components/error/ErrorComponent'
import RootComponent from './src/Views'
import VirtualBle from './src/Ble/VirtualBle.js'
// import { checkPermission, requestPermission } from 'react-native-android-permissions';

const stateTransformer = (state) => {
  if (Iterable.isIterable(state)) {
    return state.toJS()
  } else {
    return state;
  }
};

const logger = createLogger({ stateTransformer });
const store = configureStore()

export default class App extends Component {

  render() {
    return (
      <Provider store={store}>
        <View style={{flex:1}}>
          <ErrorComponent /> 
          <BleComponent />
          <RootComponent />
        </View>
      </Provider>
    );
  }
}
