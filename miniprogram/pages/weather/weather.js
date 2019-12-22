// 引用百度地图微信小程序JSAPI模块 
var bmap = require('../../libs/bmap-wx.js');
Page({
  data: {
    weatherData: [],
    src: [],
  },
  onLoad: function() {
    var that = this;
    wx.showLoading({
      title: '获取天气状况中',
    })
    // 新建百度地图对象 
    var BMap = new bmap.BMapWX({
      ak: 'i3FVUw8xe1slKucRYLmTjWOXAgh2OkV6'
    });
    var fail = function(data) {
      console.log(data)
    };
    var success = function(data) {
      wx.hideLoading()
      var weatherData = data.currentWeather[0];

      console.log(data);
      var tip = data.originalData.results[0].index;
      for(let i=0;i<5;++i)
      {
that.data.src=that.data.src.concat({title:tip[i].tipt+":",text:tip[i].des})
      }
      that.setData({
        src: that.data.src
      })
      weatherData = '城市：' + weatherData.currentCity + '\n' + 'PM2.5：' + weatherData.pm25 + '\n' + '日期：' + weatherData.date + '\n' + '温度：' + weatherData.temperature + '\n' + '天气：' + weatherData.weatherDesc + '\n' + '风力：' + weatherData.wind;
      that.setData({
        weatherData: weatherData
      });
    }
    // 发起weather请求 
    BMap.weather({
      fail: fail,
      success: success
    });
  },





  onShow: function() {
    //自动跳转到login
    /*setTimeout(function() {
      //页面跳转相当于	
      wx.switchTab({
        url: '../index/index',
      })
    }, 10000);*/
  },
  swap: function() {


    wx.switchTab({
      url: '../index/index',
    })


  }




})