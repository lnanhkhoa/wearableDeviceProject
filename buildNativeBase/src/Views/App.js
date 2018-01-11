/* @flow */

import React from "react"
import { Platform } from "react-native"
import AppNavigator from './AppNavigator.js'

export default class App extends React.Component{
  render(){
    return(
      <AppNavigator />
    );
  }  
}