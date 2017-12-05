import React, { Component } from "react";
import { Platform, View } from "react-native";
import {
  Container,
  Header,
  Title,
  Content,
  Button,
  Icon,
  ListItem,
  Text,
  Badge,
  Left,
  Right,
  Body,
  Switch,
  Radio,
  Picker,
  Separator,
  List
} from "native-base";

import styles from "./styles";

const group1 = [
  {
    key: 1,
    left: { icon:"watch", color: "blue"},
    body: { title: "Device"},
    right: {type: "Text", value: "not connected", icon:"arrow-forward"},
    route: "ConnectDevice"

  },
]
const group2 = [
  {
    key: 1,
    left: { icon:"flag", color: "#007AFF"},
    body: { title: "Sport goal"},
    right: {type: "TextIcon", value: "9000", icon:"arrow-forward"},
    route: "Devices connection"
  },
  {
    key: 2,
    left: { icon:"moon", color: "#007AFF"},
    body: { title: "Sleep goal"},
    right: {type: "TextIcon", value: "8.0h", icon:"arrow-forward"},
    route: "Devices connection"
  },
]
const group3 = [
  {
    key: 1,
    left: { icon:"phone-portrait", color: "#4CDA64"},
    body: { title: "Unit setting"},
    right: {type: "TextIcon", value: "Metric", icon:"arrow-forward"},
    route: "Devices connection"
  },
  {
    key: 2,
    left: { icon:"phone-portrait", color: "#4CDA64"},
    body: { title: "About us"},
    right: {type: "Icon", icon:"arrow-forward"},
    route: "Devices connection"
  },
];

class ListItemComponent extends Component{
  constructor(props){
    super(props)
  }

  renderRight=(right)=>{
    switch(right.type){
    case "Switch": 
      return(
        <Right>
          <Switch value={right.value} onTintColor={right.color} />
        </Right>
      )
    case "TextIcon":
      return(
        <Right>
          <Text>{right.value}</Text>
          {Platform.OS === "ios" && <Icon active name={right.icon} />}
        </Right>
      )
    case "Text":
      return(
        <Right>
          <Text>{right.value}</Text>
        </Right>
      )
    case "Icon":
      return(
        <Right>
          {Platform.OS === "ios" && <Icon active name={right.icon} />}
        </Right>
      )
    case "Radio":
      return(
        <Right>
          <Radio selected={right.value?true:false} />
        </Right>
      )
    case "Badge":
      return(
        <Right>
          <Badge style={{ backgroundColor: right.color }}>
            <Text>{right.value}</Text>
          </Badge>
        </Right>
      )
    default: return(<Right></Right>)
    }
  }

  renderItem=({key, left, body, right, route})=>{
    return(  
      <ListItem icon key={key} button onPress={() => this.props.navigation.navigate(route)}>
        <Left>
          <Button style={{ backgroundColor: left.color}}>
            <Icon active name={left.icon} />
          </Button>
        </Left>
        <Body>
          <Text>{body.title}</Text>
        </Body>
        {
          this.renderRight(right)
        }
      </ListItem>
    );
  }
  render(){
    return(
      <View>
      <ListItem itemDivider>
        <Text>{this.props.header}</Text>
      </ListItem>
      {
      this.props.data.map((item, index)=>(
        
        this.renderItem(item)
      ))
      }
      </View>
    );
  }
}


class Settings extends Component {
  constructor(props) {
    super(props);
    this.state = {
      selectedItem: undefined,
      selected1: "key1",
      results: {
        items: []
      }
    };
  }

  onValueChange(value: string) {
    this.setState({
      selected1: value
    });
  }

  render() {
    return(
      <Container style={styles.container}>
        <Header>
          <Left>
            <Button transparent onPress={() => this.props.navigation.navigate("DrawerOpen")}>
              <Icon name="menu" />
            </Button>
          </Left>
          <Body>
            <Title>Settings</Title>
          </Body>
          <Right />
        </Header>
        <Content>
          <Separator bordered noTopBorder />
          <ListItemComponent data={group1} header="My Device" navigation={this.props.navigation}/>
          <Separator noTopBorder />
          <ListItemComponent data={group2} header="Setting" navigation={this.props.navigation}/>
          <Separator noTopBorder />
          <ListItemComponent data={group3} header="More" navigation={this.props.navigation}/>
        </Content>
      </Container>
    );
  }
}

export default Settings;
