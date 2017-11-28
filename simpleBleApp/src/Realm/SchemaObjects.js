





var uuid = require('uuid-v4');

const objectRealm = {
  createNewDataSensor: function(array){
    let [stepsCount, distance, caloriesBurn, heartRate, spO2, bloodPressure, updatedAt] = array
    let newObject = {
      id: uuid(),
      stepsCount: stepsCount,
      distance: distance,
      caloriesBurn: caloriesBurn,
      heartRate: heartRate,
      spO2: spO2,
      bloodPressure: bloodPressure,
      updatedAt: updatedAt
    }
    return newObject
  },
  
  createPerson: function(realName, displayName){
    let newObject = {
      realName: realName,
      displayName: null,
      updatedAt: new Date(),
    }
    return newObject
  },
};

module.exports.objectRealm = objectRealm;