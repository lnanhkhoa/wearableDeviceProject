const React = require("react-native");

const { StyleSheet } = React;
export const primaryColor = "#42474F"
export default {
  container: {
    backgroundColor: "white"
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
  hinh:{
    alignSelf:"center",
    width:100,
    height: 100,
    borderRadius:50,
    margin: 15
  },
  username:{
    alignSelf: 'center',
    fontSize: 20,
    margin: 10,
    color: "#0026FF"
  },
  textPickerItem: {
    flex:3,
    justifyContent:'space-between',
    textAlign: 'center'
  },

};
