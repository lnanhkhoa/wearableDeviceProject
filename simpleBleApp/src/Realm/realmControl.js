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

var currentDate = new Date(2017,0,1)

const realm = new Realm({schema: [PersonSchema, MeasurementSchema]});
  realm.write(() => {
    let Person = realm.objects('Person');
    realm.delete(Person);
    let measure = realm.objects('Measurement');
    realm.delete(measure);
    while(true){
      let arrayData = Utils.createRandomData();
      realm.create('Measurement',objectRealm.createNewDataSensor(arrayData));
      if(arrayData[6] > new Date(2018,0,0)) break;
    }
  })


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
  query: function(stringQuery){
    let sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects('Measurement').filtered(stringQuery).sorted(sortBy);
    return realmAllObject.slice(0,realmAllObject.length);
  },
  save: function(todo) {
    realm.write(() => {
      realm.create('Measurement', todo);
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

module.exports = realmMeasureService;