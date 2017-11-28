 // ProfileComponent.js
 
 import React, { Component } from 'react'
 import { View, Text, StyleSheet } from 'react-native'
 import Style from '../Style'

 export default class ProfileComponent extends Component {
  render() {
    return (
      <View style={Style.component}>
        <View style={styles.element1}>
          <Text>Cân nặng: ... </Text>
          <Text>Chiều cao: ... </Text>
          <Text>Màu da: ... </Text>
          <Text>Tiền sử bệnh: ... </Text>
        </View>
        <View style={{flexDirection:'row', flex: 1}}>
          <View style={styles.element2}>
            <Text>Kế hoạch luyện tập</Text>
            <Text>Chạy bộ</Text>
            <Text>Hít thở</Text>
            <Text>Ngồi quá lâu</Text>
          </View>
          <View style={styles.element3}>
            <Text>Cảnh báo: ...</Text>
            <Text>Lời khuyên: ...</Text>
            <Text>Thời gian cập nhật: ...</Text>
          </View>
        </View>
      </View>
    );
  }
 }
 
 const styles = StyleSheet.create({
  element1:{
    flex: 1,
    alignItems: 'center',

  },
  element2:{
    flex: 1,
    alignItems: 'center',
  },
  element3:{
    flex: 1,
    alignItems: 'center',
  }
 })