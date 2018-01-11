// index.js

import configureStore from './store/configureStore.js'

import * as ble from './reducers/BleActions.js'
import SceneConst from './reducers/Const.js'
import { connect } from 'react-redux'

module.exports.configureStore = configureStore
module.exports.connect =  connect
module.exports.ble = ble
module.exports.SceneConst = SceneConst