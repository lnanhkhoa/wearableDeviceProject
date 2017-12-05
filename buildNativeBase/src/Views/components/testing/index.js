// index.js
import React, { Component } from 'react'
import { View, Text, Button } from 'react-native'

var render=0
var render1=0
var mount=0

class Testing extends Component {
	constructor(props){
		super(props)
	}

	componentWillMount(){

	}
	componentWillUnmount(){
		mount+=1
	}

	render() {
		render +=1
		return (
			<View>
				<Text>Render: {render}</Text>
				<Text>Mount: {mount}</Text>
			</View>
		);
	}
}

export default class TestingComponent extends Component {
	constructor(props){
		super(props)
	}

	render() {
		render1 +=1
		return(
				<View>
					<Text>{render1}</Text>
					{ render1%2 === 1 && <Testing /> } 
					<Button title="Button" onPress={()=>this.forceUpdate()}/>
				</View>
		);
	}
}