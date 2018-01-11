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

class TestingComponent1 extends Component {
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

import { ProgressDialog } from 'react-native-simple-dialogs';

export default class TestingComponent extends Component {
	render(){
		return(
			<ProgressDialog 
		    visible={true} 
		    title="Progress Dialog" 
		    message="Please, wait..."
			/>
		);
	}
}