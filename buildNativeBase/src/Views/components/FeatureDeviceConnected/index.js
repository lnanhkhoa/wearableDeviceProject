// index.js
import React, { Component } from 'react'
import { View } from 'react-native'
import {Container, Content, Header, Left, Body, Right, Title, Button, Icon, List, ListItem, Text} from 'native-base'
import styles from "./styles";
import { connect, ble } from '../../../Redux/'
import { realmMeasureService } from '../../../Realm/'

class FeatureDeviceConnected extends Component {
	
	_onPressButtonDisconnectDevice=()=>{
    if(this.props.state === 'CONNECTED'){
 		 this.props.changeDeviceState(this.props.selectedDeviceId, ble.DEVICE_STATE_DISCONNECT)
     realmMeasureService.deleteAllInstance()
 		 this.props.navigation.goBack()
    } else{

    }
 	}

	render() {
		return (
			<Container style={styles.container}>
        <Header>
          <Left>
            <Button transparent onPress={() => this.props.navigation.goBack()}>
              <Icon name="arrow-back" />
            </Button>
          </Left>
          <Body>
            <Title>Feature Device Connected</Title>
          </Body>
          <Right />
        </Header>
        <Content>
        	<List>
        		<ListItem onPress={this._onPressButtonDisconnectDevice}>
      				<Text>Disconnect Current Device</Text>
        		</ListItem>
        	</List>
        </Content>
      </Container>
		);
	}
}

const mapStateToProps=(state) =>({
  state: state.getIn(['ble', 'state']),
	selectedDeviceId: state.getIn(['ble', 'selectedDeviceUUID'])
})

const mapDispatchToProps = {
	changeDeviceState: ble.changeDeviceState
}


export default connect(mapStateToProps, mapDispatchToProps)(FeatureDeviceConnected)