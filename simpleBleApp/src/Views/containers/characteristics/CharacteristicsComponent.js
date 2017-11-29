'use strict';

import React, { Component } from 'react';
import { View, Text } from 'react-native';
import { connect } from 'react-redux'
import { Actions } from  'react-native-router-flux'

import * as SceneConst from '../../../Redux/reducers/Const.js'
import * as ble from '../../../Redux/reducers/BleActions.js'
import ImmutableListView from '../assets/ImmutableListView.js'
import CharacteristicView from './CharacteristicView.js'
import Style from '../../Style.js'

class CharacteristicsComponent extends Component {

  _characteristicClicked(rowData) {
    this.props.selectCharacteristic(this.props.deviceId, this.props.serviceId, rowData.get('uuid'))
    Actions[SceneConst.CHARACTERISTIC_DETAILS_SCENE]();
  }

  _renderCharacteristicCell(rowData) {
    return (
      <CharacteristicView
        isReadable={rowData.get('isReadable')}
        isWritable={rowData.get('isWritable')}
        isNotifiable={rowData.get('isNotifiable')}
        uuid={rowData.get('uuid')}
        value={rowData.get('value')}
        onClick={this._characteristicClicked.bind(this, rowData)}
      />
    )
  }

  render() {
    return (
      <View style={Style.component}>
        <ImmutableListView
          data={this.props.characteristics}
          onRenderCell={this._renderCharacteristicCell.bind(this)}/>
        <Text>Device status: {this.props.state}</Text>
      </View>
    )
  }
}

export default connect(
  state => {
    const deviceId = state.getIn(['ble', 'selectedDeviceUUID']);
    const serviceId = state.getIn(['ble', 'selectedServiceUUID']);

    return {
      deviceId,
      serviceId,
      state: state.getIn(['ble', 'state']),
      characteristics: state.getIn(['ble', 'devices', deviceId, 'services', serviceId, 'characteristics']) 
    }
  },
  {
    selectCharacteristic: ble.selectCharacteristic
  })
(CharacteristicsComponent)