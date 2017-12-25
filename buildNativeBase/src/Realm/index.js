const Realm = require('realm')
const {objectRealm, PersonSchema, HRMeasurementSchema, PedoMeasurementSchema, 
  InstanceHeartRateSchema, InstanceSpO2Schema } = require('./SchemaObjects')
const Utils = require('./Utils');

const realm = new Realm({
  schema: [
    PersonSchema, 
    HRMeasurementSchema,
    PedoMeasurementSchema,
    InstanceHeartRateSchema, 
    InstanceSpO2Schema
  ]
});

let realmMeasureService = {
  findAll: function(name:string, sortBy){
    if (!sortBy) sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects(name).sorted(sortBy);
    return realmAllObject.slice(0, realmAllObject.length);
  },
  findWithLimit: function(name:string, limit: number){
    let sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects(name).sorted(sortBy);
    return realmAllObject.slice(0, limit);    
  },
  findInstanceLimit: function(name: string, limit: number){
    let sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects(name).sorted(sortBy);
    return realmAllObject.slice(0, limit);    
  },
  query: function(name:string, stringQuery){
    let sortBy = [['updatedAt', true]];
    let realmAllObject = realm.objects(name).filtered(stringQuery).sorted(sortBy);
    return realmAllObject.slice(0,realmAllObject.length);
  },
  save: function(name:string, obj) { // auto add {id,updateAt}
    let file = objectRealm.createObject(obj)
    realm.write(() => {
      realm.create(name, file);
    })
  },
  saveInstance: function(name: string, value: number, ){
    let file = objectRealm.createInstance(value);
    realm.write(() => {
      realm.create(name, file);
    })
  },
  insertInstance: function(name: string, value: number ){
    let file = objectRealm.createInstance(value);
    realm.write(() => {
      realm.create(name, file);
    })
  },
  insertManyInstance: function(name:string, array){
    let file = null;
    realm.write(()=>{
      array.forEach(function(arr){
        file = objectRealm.createInstance(arr);
        realm.create(name, file);
      })
    })
  },
  deleteAllInstance: function(){
    realm.write(()=>{
      let InstanceHeartRate = realm.objects('InstanceHeartRate').filtered("id != '0'");
      realm.delete(InstanceHeartRate);
      let InstanceSpO2 = realm.objects('InstanceSpO2').filtered("id != '0'");
      realm.delete(InstanceSpO2);
    })
  },
  saveRandomData: function(name:string){
    let lastDate = realm.objects(name).sorted([['updatedAt', true]])[0].updatedAt
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
module.exports.realmMeasureService = realmMeasureService;