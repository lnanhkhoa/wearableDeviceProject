
var uuid = require('uuid-v4');

module.exports.PersonSchema =  PersonSchema = {
  name: 'Person',
  properties: {
    realName:     {type: 'string'}, 
    displayName:  {type: 'string?'},
    updatedAt:    {type: 'date', optional: true} 
  }
};

module.exports.HRMeasurementSchema = HRMeasurementSchema = {
  name: 'HeartRateMeasurement',
  properties: {
    id:           {type: 'string'},
    heartRate:    {type: 'int'},
    spO2:         {type: 'int'},
    updatedAt:    {type: 'date', indexed: true}
  }
};

module.exports.PedoMeasurementSchema = PedoMeasurementSchema = {
  name: 'PedometerMeasurement',
  properties: {
    id:           {type: 'string'},
    stepsCount:   {type: 'int'},
    distance:     {type: 'int'},
    caloriesBurn: {type: 'int'},
    updatedAt:    {type: 'date', indexed: true}
  }
};

module.exports.InstanceHeartRateSchema = InstanceHeartRateSchema = {
  name: 'InstanceHeartRate',
  properties: {
    id:           {type: 'string'},
    value:        {type: 'int'},
    updatedAt:    {type: 'date', indexed: true}
  }
};

module.exports.InstanceSpO2Schema = InstanceSpO2Schema = {
  name: 'InstanceSpO2',
  properties: {
    id:           {type: 'string'},
    value:        {type: 'int'},
    updatedAt:    {type: 'date', indexed: true}
  }
};

const objectRealm = {
  createPedometer: function(array){
    let [stepsCount, distance, caloriesBurn, updatedAt] = array
    let newObject = {
      id: uuid(),
      stepsCount: stepsCount,
      distance: distance,
      caloriesBurn: caloriesBurn,
      updatedAt: updatedAtstepsCount
    }
    return newObject
  },
  
  createObject:function(obj){
    let newObject = {
      id: uuid(),
      updatedAt: new Date()
    }
    Object.assign(newObject,obj)
    return newObject
  },

  createHeartRate: function(array){
    let [ heartRate, spO2, bloodPressure, updatedAt] = array
    let newObject = {
      id: uuid(),
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
  createInstance: function(value: number){
    let object = {
      id: uuid(),
      value: value,
      updatedAt: new Date()
    }
    return object
  }
};
module.exports.objectRealm = objectRealm;