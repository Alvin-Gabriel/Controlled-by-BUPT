//index.js
var app = getApp()
Page({
  data: {
    time: "点此处设置时间",
    Time: [],
    event: [],
    event1: "",
    date: "",
    tag: 1,
    userInfo: {},
    focus: false,
    inputValue: '',
    userinput: '',
  },
  timechange: function (e) {
    var edv = e.detail.value;
    this.setData({
      time: e.detail.value
    })
    console.log(this.data.time)
  },
  input: function (e) {
    this.setData({
      event1: e.detail.value
    })
    console.log(this.data.event)
  },
  save: function () {
    var _this=this;
    var db = wx.cloud.database();
    db.collection('text').add({
      data: {
        tag: 1,
        time: this.data.time,
        event: this.data.event1,
      },
      success: function (res) {
        console.log(res),
          wx.showToast({
            title: '上传成功！',
          })
        _this.setData({
          event1: "",
          time: "点此处设置时间"
        })
      },
    })
  },
  what: function () {
    this.delete1()
    var _this = this;
    wx.showLoading({
      title: '刷新中..',
      success: function (res) {
        console.log(res),
          wx.showToast({
            title: "上传成功",
          })
      },
    })
  },
  onLoad: function (options) {
    var that = this;
    wx.showLoading({
      title: '刷新中',
    })
    wx.cloud.callFunction({
      // 云函数名称
      name: 'data',
      // 传给云函数的参数
      data: {
        tag: 1
      },
      success: function (res) {
        wx.hideLoading()
        console.log(res)
        var len = res.result.data.length;
        for (let i = 0; i < len; ++i) {
          that.data.event = that.data.event.concat({ id: res.result.data[i]._id, event: res.result.data[i].event, Time: res.result.data[i].time })
        }
        that.setData({
          event: that.data.event,
        }), console.log(that.data.event)
      }
    })

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
  ButtonTap: function (e) {
    var del = e.currentTarget.dataset.id;
    console.log(del)
    var _this = this;
    wx.showModal({
      title: '提示',
      content: '确定要删除该事件吗？',
      success: function (res) {
        if (res.confirm) {
          //云函数删除
          wx.cloud.callFunction({
            name: "Delete1",
            data: {
              _id: del,
            },
            success: res => {
              wx.showToast({
                title: '删除成功!',
              }), _this.what()
              console.log('删除成功! ', res)
            },
          })
        } else if (res.cancel) {
          return false;
        }
      }
    })
  },
  delete1: function () {
    this.setData({
      event: []
    })
  },

  what: function () {
    this.delete1()
    var _this = this;
    wx.showLoading({
      title: '刷新中..',
    })
    wx.cloud.callFunction({
      // 云函数名称
      name: 'data',
      // 传给云函数的参数
      data: {
        tag: 1
      },
      success: function (res) {
        console.log(res)
        wx.hideLoading()
        var length = res.result.data.length
        for (let i = 0; i < length; ++i) {
          _this.data.event =
            _this.data.event.concat({ id: res.result.data[i]._id, event: res.result.data[i].event, Time: res.result.data[i].time });
        }
        _this.setData({
          event: _this.data.event
        })
      },
      fail: console.error
    })
    console.log(_this.data.event)
  },
})