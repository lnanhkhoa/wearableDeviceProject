// Utils.js

let Utils = {
  checkConnectDevice: function(blestate){
    if(blestate === 'CONNECTED'){
      return true
    }
    return false
  },

  devInfoServices: function(props){
    console.log(props.devices);
    // let keyServices = Object.keys(props.devices[props.selectedDeviceId].services);
    // console.log(keyServices);
  }
}

module.exports = Utils;