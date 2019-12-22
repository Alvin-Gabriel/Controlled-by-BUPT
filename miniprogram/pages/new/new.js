//new.js
const app = getApp()
var util = require('../../utils/util.js');
Page({
  data: {
    date: "",
    event: "",
    switch1: false,
    currentdate: "0",
    mon: [0,31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365],
    now: "",
    plan: "",
    dt: "",
    tag:1,
    showDialog: false,
    oneButton: [{ text: '确定' }],
    event1:""
  },

  onLoad: function () {
    var cd = util.formatTime(new Date());
    this.setData({
      currentdate: cd,
    })
    var c = this.data.currentdate;
    var ex = (c[0] - c[0] % 4 - 2000) / 4 + 1;
    if (c[0] % 4 == 0 && c[1] < 3) ex--;
    this.setData({
      now: (c[0]-2000)*365+(this.data.mon[c[1] - 1]) * 1 + c[2] * 1 + ex
    }),
      console.log(this.data.now)
  },

  print: function (e) {
    var edv = e.detail.value;
    console.log('picker发送选择改变，携带值为', edv)
    var a = new Array();
    for (var i = 0; i < 3; i++)a[i] = edv.split("-")[i];
    var ex = (a[0] - a[0] % 4 - 2000) / 4 + 1;
    if (a[0] % 4 == 0 && a[1] < 3) ex--;
    this.setData({
      plan:(a[0]-2000)*365+(this.data.mon[a[1] - 1]) * 1+a[2]*1+ex
    }),
      console.log(this.data.plan)
    var dt = this.data.plan - this.data.now;
    this.setData({
      date: a[0]+"年"+a[1]+"月"+a[2]+"日",
      dt: dt
    }), console.log(this.data.dt),console.log(this.data.date)
  },
  input: function (e) {
    console.log(e.detail.value)
    this.setData({
      event: e.detail.value
    })
  },
  switchchange: function (e) {
    this.setData({ switch1: e.detail.value })
    console.log(this.data.switch1),
      console.log(this.data.now)
  },
  confirm: function () {
    if (this.data.date != 0) {
      if (this.data.event != 0) {
        var db = wx.cloud.database();
        db.collection('require').add({
          data: {
            tag:this.data.tag,
            date: this.data.date,
            event: this.data.event,
            dt:this.data.dt,
            star: this.data.switch1,
            then:this.data.now,
          },
          success: function (res) {
            console.log(res),
              wx.showToast({
                title: '上传成功！',
              })
        
          },
        })
        wx.navigateBack({
          delta:1
        })
      }
      else {
        wx.showModal({
          title: '错误',
          content: '事件不能为空！',
          showCancel: false,
          confirmText: '好的',
          confirmColor: '',
          success: function (res) { },
          fail: function (res) { },
          complete: function (res) { },
        })
      }
    }
    else {
      wx.showModal({
        title: '错误',
        content: '日期不能为空！',
        showCancel: false,
        confirmText: '好的',
        confirmColor: '',
        success: function (res) { },
        fail: function (res) { },
        complete: function (res) { },
      })
    }
  },
  tapDialogButton(e) {
    this.setData({
      showDialog: false,
      event1:this.data.event
    })
  },
  tapButton(e) {
    this.setData({
      showDialog: true
    })
  }

})
