'use strict';

import { combineReducers } from 'redux-immutable'
import bleReducer from './BleReducer.js'
import sceneReducer from './SceneReducer.js'

export default combineReducers({
  ble:   bleReducer,
  route: sceneReducer
})
