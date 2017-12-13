import React from "react"
import { View } from 'react-native'
// import { Container, Content, Text, StyleProvider } from 'native-base';
// import getTheme from './native-base-theme/components'
import { Provider } from 'react-redux'
import App from "./src/Views/App"
import BleComponent from './src/Ble/BleComponent.js'
import ErrorComponent from './src/Ble/ErrorComponent.js'
import BleValueToRealm from './src/Ble/BleValueToRealm.js'
import {configureStore} from './src/Redux/'

// import TestingComponent from './src/Views/components/testing/'
// import Measurement from './src/Views/components/Measurement/'
const store = configureStore()
export default class App1 extends React.Component {
  constructor() {
    super();
    this.state = {
      isReady: false
    };
  }
  
  render(){
    return(
      <Provider store={store}>
        <View style={{flex:1}}>
          <ErrorComponent />
          <BleComponent />
          <BleValueToRealm />
          <App />
        </View>
      </Provider>
    );
  }
}
