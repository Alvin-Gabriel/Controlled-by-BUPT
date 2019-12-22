// miniprogram/pages/timetable/timetable.js
var util = require('../../utils/util.js');
Page({
  data: {
    time:"",
    targetTime: "",
    length: "",
    windowSwitch: -1,
    image1: "../../images/睡眠.png",
    image2: "../../images/自动化运维(1).png"
  },
  onLoad: function (options) {
    var _this = this;
    wx.showLoading({
      title: '获取当前状态中',
    })
    wx.cloud.callFunction({
      name: 'reqswitch',
      data: {
        tag: 1
      },
      success: function (res) {
        console.log(res)
        wx.hideLoading()
        var x = res.result.data[0].state;
        _this.setData({
          windowSwitch: x,
        })
      },
    })
  },
  switchimage: function () {
    var image1 = image1;
    if (image1 = "../../images/自动化运维(1).png") {
      this.setData({


        image1: "../../images/自动化运维.png"

      })
      if (image1 = "../../images/自动化运维.png") {
        image1: "../../images/自动化运维(1).png"
      }



    }


  }, switchimage1: function () {
    var image2 = image2;
    if (image2 = "../../images/睡眠选中.png") {
      this.setData({


        image2: "../../images/睡眠.png"

      })
      if (image2 = "../../images/睡眠.png") {
        image2: "../../images/睡眠选中.png"
      }



    }


  },
  /**
   * 生命周期函数--监听页面初次渲染完成
   */

  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  },
  setclock: function (e) {
    var edv = e.detail.value;
    this.setData({
      time:edv
    }),console.log(this.data.time)
    var tt = (edv[0] * 10 + edv[1] * 1) * 3600 + (edv[3] * 10 + edv[4] * 1) * 60;
    var ct = util.formatTime(new Date())[3] * 3600 + util.formatTime(new Date())[4] * 60 + util.formatTime(new Date())[5] * 1;
    if (tt >= ct) {
      this.setData({
        targetTime: tt - ct
      })
    }
    else {
      this.setData({
        targetTime: tt - ct + 86400
      })
    }
    var target = this.data.targetTime;
    var i = 0; var j = 1;
    for (; target > j; i++)j *= 10;
    this.setData({
      length: i * j + 3 * j * 10
    }), console.log(this.data.targetTime)
  },
  confirmclock: function () {
    console.log(this.data.targetTime),
      wx.request({
        url: 'https://api.heclouds.com/cmds?device_id=575232844',
        header: {
          'content-type': 'application/json',
          "api-key": "mAyh0HZIlbQlaNegMWyqgbc7KVc=",
        }, method: "POST",
        data: this.data.targetTime + this.data.length,
        success: function (res) {
          console.log(res)
          wx.showToast({
            title: '闹钟设定成功',
          })
        }
      })
  },
  window: function () {
    wx.showLoading({
      title: '发送指令中',
    })
    this.setData({
      windowSwitch: this.data.windowSwitch * -1
    })
    var db = wx.cloud.database();
    db.collection('windowSwitch').add({
      data: {
        state: this.data.windowSwitch,
        tag: 1
      },
      success: function (res) {
        console.log(res)
      },
    })
    wx.cloud.callFunction({
      name: 'reqswitch',
      data: {
        tag: 1
      },
      success: function (res) {
        console.log(res)
        var y = res.result.data[0]._id;
        wx.cloud.callFunction({
          name: "delswitch",
          data: {
            _id: y,
          },
          success: function (res) {
            wx.hideLoading()
            console.log(res)
            wx.showToast({
              title: '发送成功!',
            })
          }
        })
      }
    })

    if (this.data.windowSwitch > 0) {
      wx.request({
        url: 'https://api.heclouds.com/cmds?device_id=575232844',
        header: {
          'content-type': 'application/json',
          "api-key": "mAyh0HZIlbQlaNegMWyqgbc7KVc=",
        }, method: "POST",
        data: 1,
        success: function (res) {
          console.log(res)
        }
      })
    }
    else {
      wx.request({
        url: 'https://api.heclouds.com/cmds?device_id=575232844',
        header: {
          'content-type': 'application/json',
          "api-key": "mAyh0HZIlbQlaNegMWyqgbc7KVc=",
        }, method: "POST",
        data: 2,
        success: function (res) {
          console.log(res)
        }
      })
    }
  },
  auto: function () {
    wx.request({
      url: 'https://api.heclouds.com/cmds?device_id=575232844',
      header: {
        'content-type': 'application/json',
        "api-key": "mAyh0HZIlbQlaNegMWyqgbc7KVc=",
      }, method: "POST",
      data: 0,
      success: function (res) {
        console.log(res)
        wx.showToast({
          title: '设置成功',
        })
      }
    })
  }
})
