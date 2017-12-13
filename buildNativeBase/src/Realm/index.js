const Realm = require('realm')
const objectRealm = require('./SchemaObjects').objectRealm
const Utils = require('./Utils');

const PersonSchema = {
  name: 'Person',
  properties: {
    realName:    'string', 
    displayName: 'string?',
    updatedAt:    {type: 'date', optional: true}, 
  }
};

const MeasurementSchema = {
  name: 'Measurement',
  properties: {
    id:           {type: 'string'},
    stepsCount:     {type: 'int'},
    distance:     {type: 'int'},
    caloriesBurn:   {type: 'int'},
    heartRate:  {type: 'int'},
    spO2:       {type: 'int'},
    bloodPressure:  {type: 'int'},
    updatedAt:    {type: 'date', indexed: true}
  }
};

const InstanceHeartRateSchema = {
  name: 'Instance_HeartRate',
  properties: {
    id: {type: 'string'},
    value: {type: 'int'},
    updatedAt: {type: 'date', indexed: true}
  }
}
const InstanceStepCountSchema = {
  name: 'Instance_StepCount',
  properties: {
    id: {type: 'string'},
    value: {type: 'int'},
    updatedAt: {type: 'date', indexed: true}
  }
}

const realm = new Realm({schema: [
  PersonSchema, 
  MeasurementSchema, 
  InstanceHeartRateSchema, 
  InstanceStepCountSchema]
});

let realmMeasureService = {
  findAll: function(sortBy){
    if (!sortBy) sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects('Measurement').sorted(sortBy);
    return realmAllObject.slice(0, realmAllObject.length);
  },
  findWithLimit: function(limit){
    let sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects('Measurement').sorted(sortBy);
    return realmAllObject.slice(0, limit);    
  },
  findInstanceLimit: function(name: string, limit: number){
    let sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects(name).sorted(sortBy);
    return realmAllObject.slice(0, limit);    
  },
  query: function(stringQuery){
    let sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects('Measurement').filtered(stringQuery).sorted(sortBy);
    return realmAllObject.slice(0,realmAllObject.length);
  },
  save: function(file) {
    realm.write(() => {
      realm.create('Measurement', file);
    })
  },
  saveInstance: function(name: string, value: number){
    let file = objectRealm.createInstance(value);
    realm.write(() => {
      realm.create(name, file);
    })
  },
  deleteAllInstance: function(){
    realm.write(()=>{
      let Instance_HeartRate = realm.objects('Instance_HeartRate');
      realm.delete(Instance_HeartRate);
      let Instance_StepCount = realm.objects('Instance_StepCount');
      realm.delete(Instance_StepCount);
    })
  },
  saveRandomData: function(){
    let lastDate = realm.objects('Measurement').sorted([['updatedAt', true]])[0].updatedAt
    this.save(objectRealm.createNewDataSensor(Utils.createRandomData()))
  },
  update: function(todo, callback) {
    if (!callback) return;
    realm.write(() => {
      callback();
      todo.updatedAt = new Date();
    });
  }
}

// realmMeasureService.deleteAllInstance()

module.exports.realmMeasureService = realmMeasureService;