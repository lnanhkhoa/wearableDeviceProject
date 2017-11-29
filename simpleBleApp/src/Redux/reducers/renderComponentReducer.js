// renderComponentReducer.js
'use strict';

import * as ble from './updateActions'
import { Map, List, OrderedMap } from 'immutable'

const defaultState = Map({
	updateComponent: Map()
});

export default (state = defaultState, action) => {
  switch (action.type) {
    case ble.HOME_COMPONENT:
      return state.updateComponent.merge(Map({home: true}));
    default:
      return state;
  }
}
