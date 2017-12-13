import { createStore, applyMiddleware, compose } from 'redux';
import thunk from 'redux-thunk';
import { composeWithDevTools } from 'remote-redux-devtools';
import reducers from '../reducers/';

export default function configureStore(initialState) {
  const store = createStore( reducers, initialState, 
    composeWithDevTools(
      applyMiddleware(thunk),
    )
  );


  return store;
};
