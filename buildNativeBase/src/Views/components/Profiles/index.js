// index.js
import React, {Component} from 'react'
import {View, Image, FlatList, Platform} from 'react-native'
import { Container, Header, Title, Content, Button, Icon, List, ListItem,
        Text, Thumbnail, Left, Right, Body, ActionSheet, Picker, Form, Item as FormItem
} from "native-base"
import styles, {primaryColor} from './styles.js'

const Item = Picker.Item;
const mount = 0
const uriImage = require("../images/logoMemsTech.png")
const username = "MEMSTECH"
const BUTTONS = []
const DESTRUCTIVE_INDEX = 3;
const CANCEL_INDEX = 4;

const datas = [
  {
    key: 0,
    text: "Gender",
    rightText: "Female",
    press: "Update Time",
    icon: "arrow-forward",
    typeOption:"",
    Options: [
      {text: "Option 0", icon: "american-football", iconColor: "#2c8ef4", value: '1', key: 1},
      {text: "Option 1", icon: "analytics", iconColor: "#f42ced", value: '2', key: 2},
      {text: "Option 2", icon: "aperture", iconColor: "#ea943b", value: '3', key: 3},
      {text: "Delete", icon: "trash", iconColor: "#fa213b", value: '4', key: 4},
      {text: "Cancel", icon: "close", iconColor: "#25de5b", value: '5', key: 5}
    ]
  },
  {
    key: 1,
    text: "Date of birth",
    rightText: "updated at",
    press: "Clear Cache",
    icon: "arrow-forward",
    Options: [
      {text: "Option 0", icon: "american-football", iconColor: "#2c8ef4", value: '1', key: 1},
      {text: "Option 1", icon: "analytics", iconColor: "#f42ced", value: '2', key: 2},
      {text: "Option 2", icon: "aperture", iconColor: "#ea943b", value: '3', key: 3},
      {text: "Delete", icon: "trash", iconColor: "#fa213b", value: '4', key: 4},
      {text: "Cancel", icon: "close", iconColor: "#25de5b", value: '5', key: 5}
    ]
  },
  {
    key: 2,
    text: "Height",
    rightText: "170 cm",
    press: "Clear Cache",
    icon: "arrow-forward",
    Options: [
      {text: "Option 0", icon: "american-football", iconColor: "#2c8ef4", value: '1', key: 1},
      {text: "Option 1", icon: "analytics", iconColor: "#f42ced", value: '2', key: 2},
      {text: "Option 2", icon: "aperture", iconColor: "#ea943b", value: '3', key: 3},
      {text: "Delete", icon: "trash", iconColor: "#fa213b", value: '4', key: 4},
      {text: "Cancel", icon: "close", iconColor: "#25de5b", value: '5', key: 5}
    ]
  },
  {
    key: 4,
    text: "Weight",
    rightText: "60.0 kg",
    press: "Clear Cache",
    icon: "arrow-forward",
    Options: [
      {text: "Option 0", icon: "american-football", iconColor: "#2c8ef4", value: '1', key: 1},
      {text: "Option 1", icon: "analytics", iconColor: "#f42ced", value: '2', key: 2},
      {text: "Option 2", icon: "aperture", iconColor: "#ea943b", value: '3', key: 3},
      {text: "Delete", icon: "trash", iconColor: "#fa213b", value: '4', key: 4},
      {text: "Cancel", icon: "close", iconColor: "#25de5b", value: '5', key: 5}
    ]
  },
]

const Utils = {
  generateElementPicker: function(item, selectedValue, onValueChange){
    function generateItemPicker(options){
      return options.map((option)=>(
        <Picker.Item label={option.text} value={option.value} key={option.key}/>
      ))
    }
    return(
      <View style={{flexDirection:'row', alignItems: 'center'}}>
        <Text style={styles.textPickerItem}>{item.text}</Text>
        <Picker
          style={{flex:1}}
          selectedValue={selectedValue}
          onValueChange={onValueChange}
        >
          {generateItemPicker(item.Options)}
        </Picker>
      </View>
    );
  }
}

export default class Profile extends Component {
  constructor(props) {
    super(props)
    this.state = {
      datas: [],
      selectedItem: undefined,
      selected1: 'key1',
      results: {
        items: []
      }
    }
  }

  pressExecute() {
  }

  onValueChange(value: string) {
    this.setState({
      selected1: value,
      lang: null
    });
  }
  changeState(value){
    this.setState({lang: value})
  }

  render() {
    return (
      <Container style={styles.container}>
        <Header hasTabs
                style={styles.header}
                androidStatusBarColor={primaryColor}
        >
          <Left>
            <Button transparent onPress={() => this.props.navigation.navigate("DrawerOpen")}>
              <Icon name="menu"/>
            </Button>
          </Left>
          <Body>
          <Title>Profile</Title>
          </Body>
          <Right/>
        </Header>
        <Content>
          <FlatList
            data={this.state.datas}
            renderItem={({item}) => {
                return (Utils.generateElementPicker(item, this.state.lang, this.changeState))
            }}
          />
        </Content>
      </Container>
    );
  }

  componentDidMount() {
    this.setState({
      datas: datas
    })
  }
}

