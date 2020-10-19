//index.js
//获取应用实例
const app = getApp()

function inArray(arr, key, val) {
  for (let i = 0; i < arr.length; i++) {
    if (arr[i][key] === val) {
      return i;
    }
  }
  return -1;
}

// ArrayBuffer转16进度字符串示例
function ab2hex(buffer) {
  var hexArr = Array.prototype.map.call(
    new Uint8Array(buffer),
    function (bit) {
      return ('00' + bit.toString(16)).slice(-2)
    }
  )
  return hexArr.join('');
}

//ASCII码转16进制
function strToHexCharCode(str) {
  if (str === "") {
      return "";
  } else {
      var hexCharCode = [];
      hexCharCode.push("0x");
      for (var i = 0; i < str.length; i++) {
          hexCharCode.push((str.charCodeAt(i)).toString(16));
      }
      return hexCharCode.join("");
  }
}

// 字符串转byte
function stringToBytes(str) {
  var array = new Uint8Array(str.length);
  for (var i = 0, l = str.length; i < l; i++) {
    array[i] = str.charCodeAt(i);
  }
  console.log(array);
  return array.buffer;
}  

Page({
  data: {
    devices:[],
    connected: false,
    chs: [],
    sendData:"",
    logs: [],
    dataType: true,//false: HEX type, true: ASCII type
  },

  printLog:function(log) {
    var logs = this.data.logs;
    logs.push(log);
    this.setData({log_list: logs.join('\n')})
  },

  printInfo:function(info) {
    wx.showToast({
      title: info,
      icon: 'none',
      duration: 1200,
      mask: true
    })
  },

  printTimer:function() {
    var timestamp = Date.parse(new Date())
    timestamp = timestamp / 1000;
    var n = timestamp * 1000;  
    var date = new Date(n);  
    //年  
    var Y = date.getFullYear();  
    //月  
    var M = (date.getMonth() + 1 < 10 ? '0' + (date.getMonth() + 1) : date.getMonth() + 1);  
    //日  
    var D = date.getDate() < 10 ? '0' + date.getDate() : date.getDate();  
    //时  
    var h = date.getHours();  
    //分  
    var m = date.getMinutes();  
    //秒  
    var s = date.getSeconds();
    this.printLog("当前时间：" +Y+"-"+M+"-"+D+" "+h+":"+m+":"+s)
  },

  // 开始发现设备处理函数
  startBluetoothDevicesDiscovery() {
    if(this._discoveryStarted) {
      this.printLog("已经正在发现设备...")
      return
    }
    this._discoveryStarted = true
    wx.startBluetoothDevicesDiscovery({
      allowDuplicatesKey: true,
      success: (res) => {
        this.printLog("开始发现设备...")
        this.onBluetoothDeviceFound()
      },
    })
  },

  // 停止发现设备处理函数
  stopBluetoothDevicesDiscovery() {
    this.printLog('停止发现设备')
    this._discoveryStarted = false
    wx.stopBluetoothDevicesDiscovery()
  },

  // 正在查找设备
  onBluetoothDeviceFound() {
    this.printLog('正在发现设备...')
    wx.onBluetoothDeviceFound((res) => {
      res.devices.forEach(device => {
        if (!device.name && !device.localName) {
          return
        }
        const foundDevices = this.data.devices
        const idx = inArray(foundDevices, 'deviceId', device.deviceId)
        const data = {}

        if (idx === -1) {
          data[`devices[${foundDevices.length}]`] = device
        } else {
          data[`devices[${idx}]`] = device
        }
        this.setData(data)
      })
    })
  },

  // 创建连接
  bindcreateBLEConnection(e) {
    const ds = e.currentTarget.dataset
    const deviceId = ds.deviceId
    const name = ds.name
    this.printLog("开始连接设备 [" + name + "]")
    wx.createBLEConnection({
      deviceId,
      success: (res) => {
        this.setData({
          connected: true,
          name,
          deviceId,
        })
        this.getBLEDeviceServices(deviceId)
      }
    })
    // this.stopBluetoothDevicesDiscovery()
  },

  // 断开连接
  closeBLEConnection() {
    this.printLog("断开连接")
    this.printInfo("成功断开设备")
    wx.closeBLEConnection({
      deviceId: this.data.deviceId
    })
    this.setData({
      devices: [],
      connected: false,
      chs: [],
      canWrite: false,
    })
  },

  // 获取要连接设备的服务
  getBLEDeviceServices(deviceId) {
    this.printLog("获取设备服务: " + deviceId)
    wx.getBLEDeviceServices({
      deviceId,
      success: (res) => {
        for (let i = 0; i < res.services.length; i++) {
          if (res.services[i].isPrimary) {
            this.getBLEDeviceCharacteristics(deviceId, res.services[i].uuid)
            return
          }
        }
      }
    })
  },

  // 获取要连接设备的属性
  getBLEDeviceCharacteristics(deviceId, serviceId) {
    this.printLog('开始获取设备属性: ' + deviceId + serviceId)
    wx.getBLEDeviceCharacteristics({
      deviceId,
      serviceId,
      success: (res) => {
        this.printLog('成功获取设备属性')
        for (let i = 0; i < res.characteristics.length; i++) {
          let item = res.characteristics[i]
          if (item.properties.read) {
            wx.readBLECharacteristicValue({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
            })
          }
          if (item.properties.write) {
            this.setData({
              canWrite: true
            })
            this._deviceId = deviceId
            this._serviceId = serviceId
            this._characteristicId = item.uuid
            // this.writeBLECharacteristicValue()
          }
          if (item.properties.notify || item.properties.indicate) {
            wx.notifyBLECharacteristicValueChange({
              deviceId,
              serviceId,
              characteristicId: item.uuid,
              state: true,
            })
          }
        }
      },
      fail(res) {
        this.printLog('设备属性获取失败')
      }
    })
    // 操作之前先监听，保证第一时间获取数据
    wx.onBLECharacteristicValueChange((characteristic) => {
      const idx = inArray(this.data.chs, 'uuid', characteristic.characteristicId)
      const data = {}
      if (idx === -1) {
        data[`chs[${this.data.chs.length}]`] = {
          uuid: characteristic.characteristicId,
          value: ab2hex(characteristic.value)
        }
      } else {
        data[`chs[${idx}]`] = {
          uuid: characteristic.characteristicId,
          value: ab2hex(characteristic.value)
        }
      }
      data[`chs[${this.data.chs.length}]`] = {
        uuid: characteristic.characteristicId,
        value: ab2hex(characteristic.value)
      }
      this.setData(data)
    })
  },

  writeBLECharacteristicValue() {
    // 向蓝牙设备发送一个0x00的16进制数据
    // let buffer = new ArrayBuffer(1)
    // let dataView = new DataView(buffer)
    // dataView.setUint8(0, Math.random() * 255 | 0)
    var that = this;
    if(this.data.dataType)
    {
      var buffer = stringToBytes(this.data.sendData)
      that.printLog("发送数据：" + this.data.sendData)
      console.log("ascii")
    }
    else
    {
      // var buffer = strToHexCharCode("rice")
      // this.printLog("16: " + buffer)
      console.log("hex")
    }

    wx.writeBLECharacteristicValue({
      deviceId: this._deviceId,
      serviceId: this._serviceId,
      characteristicId: this._characteristicId,
      value: buffer,
      success (res) {
        // that.printLog("发送数据：" + that.data.sendData)
        // that.printLog("发送数据成功");
      },
      fail (res) {
        that.printLog("发送数据失败")
      }
    })
  },

  // 获取写数据控件的数据
  bindWriteData (e) {
      this.setData({
        sendData: e.detail.value
      })
  },

  // 清空打印信息
  bindClearLog (e) {
    this.setData({
      logs: [],
      log_list: [],
    })
  },

  // 发送数据类型选择
  dataTypeSelect(e) {
    var that = this
    if(e.detail.value == "hex")
    {
      that.data.dataType = false
      that.printLog("dataType: HEX")
    }
    else
    {
      that.data.dataType = true
      that.printLog("dataType: ASCII")
    }
  },

  // 开始扫描按键
  openBluetoothAdapter() {
    this.printLog("启动蓝牙适配器...");

    this.setData({
      devices: [],
      connected: false,
      chs: [],
      canWrite: false,
    })

    wx.openBluetoothAdapter({
      success: (res) => {
        this.printLog("蓝牙启动成功，开始进入发现设备");
        this.startBluetoothDevicesDiscovery()
      },
      fail: (res) => {
        this.printInfo("请先打开蓝牙")
        if (res.errCode === 10001) {
          wx.onBluetoothAdapterStateChange(function (res) {
            if (res.available) {
              this.printLog("蓝牙启动成功，开始进入发现设备");
              this.startBluetoothDevicesDiscovery()
            }
          })
        }
      }
    })
  },

  // 停止扫描按键
  closeBluetoothAdapter() {
    this.printLog("停止扫描");
    wx.closeBluetoothAdapter()
    this.stopBluetoothDevicesDiscovery()
    this._discoveryStarted = false
  },

  onLoad: function () {
    this.printInfo("欢迎使用R-Plan BLE 调试小程序");
  },
})
