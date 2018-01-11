import PropTypes from 'prop-types';
import { StackNavigator, DrawerNavigator, addNavigationHelpers } from "react-navigation";
import { connect } from 'react-redux';

import Drawer from "./Drawer";
import Home from './components/home/'
import ConnectDevice from './components/connectYourDevice/'
import FeatureDeviceConnected from './components/FeatureDeviceConnected/'

export default AppNavigator = StackNavigator({
    Home: { screen: Home },
    ConnectDevice: {screen: ConnectDevice},
    FeatureDeviceConnected: {screen: FeatureDeviceConnected},
    Drawer: {screen: Drawer}
  },{
    initialRouteName: "Home",
    headerMode: "none"
});