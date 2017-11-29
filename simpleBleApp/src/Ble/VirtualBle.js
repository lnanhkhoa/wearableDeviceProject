// Chuc nang
// 1. lay data tu web ve ( 5s lay 1 lan)
// 2. Save no vo database
// 3. tao 1 reducer keu thang homeComponent can update
import React, { Component } from 'react';
import realmMeasureService from '../Realm/realmControl.js'
import TimerMixin from 'react-timer-mixin';

export default class VirtualBle extends Component {
  constructor(props){
    super(props)
    this.state={
      intervalId: null
    }
  }
  getJSONhttp(){
    fetch("https://apimemsitech.herokuapp.com/api")
    .then((response)=>response.json())
    .then((responseJson)=>{
      console.log(responseJson)
    })
    .catch((e) => {console.log(e)})
  }

  intervalSaveData=()=>{
    let id = setInterval(()=>{
      this.getJSONhttp()
    }, 5000)

    this.setState({
      intervalId: id
    })
  }
  render() {
    return(null);
  }
}

