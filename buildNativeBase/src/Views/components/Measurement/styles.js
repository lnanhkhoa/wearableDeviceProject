const React = require("react-native");

const { StyleSheet } = React;
export const primaryColor = "#42474F"
export default {
  container: {
    backgroundColor: primaryColor
  },
  header:{
    backgroundColor: primaryColor,
  },
  tabBar:{
    backgroundColor: primaryColor
  },
  tabBarUnderlineStyle:{
    backgroundColor: primaryColor*0.7,
  },
  card:{
    flex:1,    
    justifyContent: 'space-between',
    alignItems: 'center',
    paddingTop: 10,
    padding: 10,
    "backgroundColor": "white"
  },
  text: {
    alignSelf: "center",
    marginBottom: 7
  },
  mb: {
    marginBottom: 15
  },
};
