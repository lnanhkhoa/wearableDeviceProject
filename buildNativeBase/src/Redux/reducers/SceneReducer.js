import { AppNavigator } from '../../Views/AppNavigator.js'
import { NavigationActions } from "react-navigation";


console.log(AppNavigator)
let initNavState = AppNavigator.router.getStateForAction(
NavigationActions.init()
);

export default (state = initNavState, action) => {
  const newState = AppNavigator.router.getStateForAction(action, state) 
  return newState || state;
};