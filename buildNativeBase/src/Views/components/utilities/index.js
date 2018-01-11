import React, { Component } from "react";

import {
  Container,
  Header,
  Title,
  Content,
  Button,
  Icon,
  List,
  ListItem,
  Text,
  Thumbnail,
  Left,
  Right,
  Body
} from "native-base";
import styles, {primaryColor} from "./styles";
import { connect, ble } from '../../../Redux/'

const datas = [
  {
    text: "Update Time for Smartwatch",
    note: "",
    time: "3:43 pm",
    press: "Update Time"
  },
  {
    text: "Clear Cache",
    note: "",
    time: "3:43 pm",
    press: "Update Time"
  },
];

class Utilities extends Component {
  render() {
    return (
      <Container style={styles.container}>
        <Header hasTabs 
          style={styles.header}
          androidStatusBarColor={primaryColor}
        >
          <Left>
            <Button transparent onPress={() => this.props.navigation.navigate("DrawerOpen")}>
              <Icon name="menu" />
            </Button>
          </Left>
          <Body>
            <Title>Utilities</Title>
          </Body>
          <Right />
        </Header>

        <Content>
          <List
            dataArray={datas}
            renderRow={data =>
              <ListItem avatar onPress={()=>console.log(data.press)}>
                <Body>
                  <Text>{data.text}</Text>
                  <Text numberOfLines={1} note>{data.note}</Text>
                </Body>
                <Right>
                  <Text note>{data.time}</Text>
                </Right>
              </ListItem>}
          />
        </Content>
      </Container>
    );
  }
}

const mapStateToProps=(state) =>({
  selectedDeviceId: state.getIn(['ble', 'selectedDeviceUUID']),
  sceneState: state.getIn(['route', 'state'])
})

const mapDispatchToProps = {
  changeDeviceState: ble.changeDeviceState
}

export default connect(mapStateToProps, mapDispatchToProps)(Utilities);
