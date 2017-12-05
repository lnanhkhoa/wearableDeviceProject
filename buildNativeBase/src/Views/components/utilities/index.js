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

import styles from "./styles";
const datas = [
  {
    text: "Update Time for Smartwatch",
    note: "",
    time: "3:43 pm",
    press: "Update Time"
  },
];

class Utilities extends Component {
  render() {
    return (
      <Container style={styles.container}>
        <Header>
          <Left>
            <Button transparent onPress={() => this.props.navigation.navigate("DrawerOpen")}>
              <Icon name="menu" />
            </Button>
          </Left>
          <Body>
            <Title>List Avatar</Title>
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

export default Utilities;
