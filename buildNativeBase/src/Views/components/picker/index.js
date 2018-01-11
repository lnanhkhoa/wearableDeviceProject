// index.js
import React, {Component} from 'react'
import {View, Image, FlatList, Platform} from 'react-native'
import { Container, Header, Title, Content, Button, Icon, List, ListItem,
  Text, Thumbnail, Left, Right, Body, ActionSheet, Picker, Form, Item as FormItem
} from "native-base"
import styles, {primaryColor} from './styles.js'
import datas from './data.js'

const Item = Picker.Item;
const mount = 0
const uriImage = require("../images/logoMemsTech.png")
const username = "MEMSTECH"
const BUTTONS = []
const DESTRUCTIVE_INDEX = 3;
const CANCEL_INDEX = 4;

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

  componentDidMount(){
    this.setState({
      datas:datas
    });
  }

  generateRightElementSelect(){
    function generateItemPicker(options){
      return options.map((option)=>(
        <Picker.Item label={option.text} value={option.value} key={option.key}/>
      ))
    }
    return(
      <Picker
        style={{flex:1}}
        // selectedValue={}
        // onValueChange={onValueChange}
      >
        {generateItemPicker(item.Options)}
      </Picker>
    )
  }
  generateRightElement(item){
    switch(item.typeOption){
      case 'select':
        return this.generateRightElementSelect();
        break;
    }
  }

  render(){
    return(
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
              return (
                <View style={{flex:1, flexDirection:'row'}}>
                  <Text style={{flex:3}}>{item.text}</Text>
                  <View style={{flex:2}}>
                    {
                      this.generateRightElement(item)
                    }
                  </View>
                </View>
              )
            }}
          />
        </Content>
      </Container>
    );
  }


}

