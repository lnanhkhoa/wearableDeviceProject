'use strict';

import { combineReducers } from 'redux-immutable'
import bleReducer from './BleReducer.js'
import componentsReducer from './renderComponentReducer.js'

export default combineReducers({
  ble:   bleReducer,
  components: componentsReducer
})
