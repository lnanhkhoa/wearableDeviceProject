'use strict';

import React, { Component } from 'react';
import { View, AppRegistry } from 'react-native';
import { Iterable } from 'immutable';
import { srclyMiddleware, createStore } from 'redux';
import { Provider } from 'react-redux';
import { createLogger } from 'redux-logger';
import configureStore from './src/Redux/store/configureStore'

import BleComponent from './src/Ble/BleComponent'
import ErrorComponent from './src/Views/components/error/ErrorComponent'
import RootComponent from './src/Views'
import { checkPermission, requestPermission } from 'react-native-android-permissions';

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

  componentDidMount() {
    this.checkAndGrantPermissions();
  }

  checkAndGrantPermissions() {
    checkPermission("android.permission.ACCESS_COARSE_LOCATION").then((result) => {
      console.log("Already Granted!");
      console.log(result);
    }, (result) => {
      console.log("Not Granted!");
      console.log(result);
      requestPermission("android.permission.ACCESS_COARSE_LOCATION").then((result) => {
        console.log("Granted!", result);
      }, (result) => {
        console.log("Not Granted!");
        console.log(result);
      });
    });
  }

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
