/* @flow */

import React from "react";

import { Platform } from "react-native";
import { Root } from "native-base";
import { StackNavigator } from "react-navigation";

import Drawer from "./Drawer";
import Header from "./components/Header/";
import Header1 from "./components/Header/1";
import Home from './components/home/'
import ConnectDevice from './components/connectYourDevice/'

const AppNavigator = StackNavigator(
  {
    Home: { screen: Home },
    ConnectDevice: {screen: ConnectDevice},
    Drawer: {screen: Drawer}
  },
  {
    initialRouteName: "Home",
    headerMode: "none",
  }
);

export default () =>
  <Root>
    <AppNavigator />
  </Root>;
