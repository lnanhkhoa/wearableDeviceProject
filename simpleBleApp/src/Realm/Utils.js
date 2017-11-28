


var currentDate = new Date(2017,0,1);

let Utils = {
  increaseDate: function(currentDate){
    if(currentDate instanceof Date && !isNaN(currentDate.valueOf())){
      let date = new Date(currentDate);
      date.setDate(date.getDate() + 1);
      return date;
    }
  },
  random: function(min, max){
    return Math.random()*(max-min)|0 + min;
  },
  createRandomData: function(){
    let stepsCount = this.random(0, 500),
        distance = this.random(10, 20),
        caloriesBurn = this.random(20, 30),
        heartRate = this.random(60, 100),
        spO2 = this.random(94, 100),
        bloodPressure = this.random(60, 160),
        updateAt = this.increaseDate(currentDate);
    currentDate = updateAt;
    return [stepsCount, distance, caloriesBurn, heartRate, spO2, bloodPressure, currentDate];
  },
  move: function(array, fromIndex, toIndex) {
    return array.splice(toIndex, 0, array.splice(fromIndex, 1)[0]);
  },
}

module.exports = Utils;