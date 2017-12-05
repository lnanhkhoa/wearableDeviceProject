/* @flow */

import React from "react";
import { DrawerNavigator } from "react-navigation";

import Information from "./components/information/";
import Measurement from "./components/Measurement/";
import SideBar from "./components/sidebar";
import Home from './components/home/';
import Utilities from './components/utilities/'
import Settings from './components/settings/'


const DrawerExample = DrawerNavigator(
  {
    Information: { screen: Information },
    Measurement: { screen: Measurement },
    Utilities: { screen: Utilities },
    Settings: { screen: Settings },
  },
  {
    initialRouteName: "Information",
    contentOptions: {
      activeTintColor: "#e91e63"
    },
    contentComponent: props => <SideBar {...props} />
  }
);

export default DrawerExample;
