// miniprogram/pages/deadline/deadline.js
var app = getApp();
var util = require('../../utils/util.js');
Page({

  /**
   * 页面的初始数据
   */
  data: {
    mon: [0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365],
    pcontent: [],
    content: [],
    id: [],
    ct:7300,
    del: "",
    pf: 1,
    past: "past",
    future: "future",
    disabled2: false,
    disabled1: true,

  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function(options) {
    this.what()
    this.setData({
      slideButtons: [{
        type: 'warn',
        text: '删除',
        extClass: 'test',
      }],
    });
  },
  slideButtonTap(e) {
    var del = this.data.del;
    var _this = this;
    wx.showModal({
      title: '提示',
      content: '确定要删除该事件吗？',
      success: function(res) {
        if (res.confirm) {
          //云函数删除
          wx.cloud.callFunction({
            name: "Delete",
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


  navy: function() {
    wx.navigateTo({
      url: '../new/new',
      success: function(res) {},
      fail: function(res) {},
      complete: function(res) {},
    })
  },
  delete1: function() {
    this.setData({
      content: [],
      pcontent: []
    })
  },

  what: function() {
    this.delete1()
    var _this = this;
    var c = util.formatTime(new Date());
    var ex = (c[0] - c[0] % 4 - 2000) / 4 + 1;
    if (c[0] % 4 == 0 && c[1] < 3) ex--;
    var now = (c[0] - 2000) * 365 + (this.data.mon[c[1] - 1]) * 1 + c[2] * 1 + ex
    this.setData({
      ct: now
    })
    console.log(this.data.ct)
    wx.showLoading({
      title: '刷新中..',
    })
    wx.cloud.callFunction({
      // 云函数名称
      name: 'require',
      // 传给云函数的参数
      data: {
        tag: 1
      },
      success: function(res) {
        console.log(res)
        wx.hideLoading()
        var length = res.result.data.length
        var sum = 0;
        for (let i = 0; i < length; ++i) {
          _this.data.id = _this.data.id.concat(res.result.data[i]._id)
    if ((res.result.data[i].dt-_this.data.ct+res.result.data[i].then) >= 0) 
          {
            _this.data.content =
            _this.data.content.concat({
            id: res.result.data[i]._id,
            event: res.result.data[i].event,
            date: "(" + res.result.data[i].date + ")",
            when: "还有",
        dt: res.result.data[i].dt - _this.data.ct + res.result.data[i].then,
                tian: "天",
                star: res.result.data[i].star
              });
          } else {
            _this.data.pcontent =
              _this.data.pcontent.concat({
                id: res.result.data[i]._id,
                event: res.result.data[i].event,
                date: "(" + res.result.data[i].date + ")",
                when: "已过去了",
           dt: -res.result.data[i].dt+_this.data.ct-res.result.data[i].then,
                tian: "天",
                star: res.result.data[i].star
              });
            sum++;
          }
        }
        var t = {
          id: "",
          event: "",
          date: "",
          when: "",
          dt: "",
          star: ""
        }
        for (let j = 1; j < length - sum; ++j) {
          for (let k = 0; k < length - 1 - sum; ++k) {
            if (_this.data.content[k].dt > _this.data.content[k + 1].dt) {
              t = _this.data.content[k];
              _this.data.content[k] = _this.data.content[k + 1];
              _this.data.content[k + 1] = t;
            }
          }
        }
        for (let j = 1; j < sum; ++j) {
          for (let k = 0; k < sum - 1; ++k) {
            if (_this.data.pcontent[k].dt > _this.data.pcontent[k + 1].dt) {
              t = _this.data.pcontent[k];
              _this.data.pcontent[k] = _this.data.pcontent[k + 1];
              _this.data.pcontent[k + 1] = t;
            }
          }
        }
        _this.setData({
          pcontent: _this.data.pcontent,
          content: _this.data.content,
          id: _this.data.id
        })
      },
      fail: console.error
    })
  },
  touchend: function(e) {
    this.setData({
      del: e.currentTarget.dataset.id
    })
  },
  past: function () {
    console.log(0)
    this.setData({
      pf: 0,
      disabled1: false,
      disabled2: true,
    })
  },
  future: function () {
    this.setData({
      pf: 1,
      disabled2: false,
      disabled1: true,
    })
  }

})