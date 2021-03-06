import React, { Component } from "react";
import { Image, View, StatusBar } from "react-native";

import { Container, Button, H3, Text, Header, Title, Body, Left, Right } from "native-base";
import styles from "./styles";

const launchscreenBg = require("./img/launchscreen-bg.png");
const launchscreenLogo = require("./img/logo-kitchen-sink.png");

class Home extends Component {
	// eslint-disable-line

	render() {
		return (
			<Container>
				<StatusBar barStyle="light-content" />
				<Image source={launchscreenBg} style={styles.imageContainer}>
					<View style={styles.logoContainer}>
						<Image source={launchscreenLogo} style={styles.logo} />
					</View>
					<View
						style={{
							alignItems: "center",
							marginBottom: 50,
							backgroundColor: "transparent",
						}}
					>
						<H3 style={styles.text}>Welcome to MEMSFIT</H3>
						<View style={{ marginTop: 8 }} />
						<H3 style={styles.text}>Design by MemsTech</H3>
						<View style={{ marginTop: 8 }} />
					</View>
					<View style={{ marginBottom: 80 }}>
						<Button
							style={styles.button}
							onPress={() => this.props.navigation.navigate("ConnectDevice")}
						>
							<Text>Lets Start!</Text>
						</Button>
					</View>
				</Image>
			</Container>
		);
	}
}

export default Home;
