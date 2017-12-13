'use strict';

import { combineReducers } from 'redux-immutable'
import bleReducer from './BleReducer.js'

export default combineReducers({
  ble:   bleReducer
})
