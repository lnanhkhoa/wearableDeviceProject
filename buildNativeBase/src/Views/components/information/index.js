// index.js
import React, { Component } from 'react'
import {
  Container,Header, Title, Content, Button, Icon, List, 
  ListItem, Text, Thumbnail, Left, Right, Body
} from "native-base";
import { View } from 'react-native'
import styles from "./styles";

export default class Information extends Component {
	render() {
		return (
			<Container style={styles.container}>
        <Header>
          <Left>
            <Button 
            	transparent 
            	onPress={() => this.props.navigation.navigate("DrawerOpen")}
          	>
              <Icon name="menu" />
            </Button>
          </Left>
          <Body>
            <Title>Information</Title>
          </Body>
          <Right />
        </Header>
        <Content>
        	<View style={{flexDirection:'row'}}>
        	<View style={{flex:1,}}>
              <Text style={{textAlign : 'center'}}>Số bước chân gần nhất</Text>
              <Text style={{textAlign : 'center'}}>12</Text>
            </View>
            <View style={{flex:1,}}>
              <Text style={{textAlign : 'center'}}>Nhịp tim gần nhất</Text>
              <Text style={{textAlign : 'center'}}>13</Text>
            </View>
        	</View>
        	<View style={{flexDirection:'column'}}>
            <Text style={{textAlign : 'center', alignItems: 'center',}}>Bước chân</Text>
            <Text style={{textAlign : 'center', height: 100}}>Nhịp tim & SpO2</Text>            
            <Text style={{textAlign : 'center', height: 100}}>Giấc ngủ (coming soon)</Text>
          </View>
        </Content>
      </Container>
		);
	}
}
