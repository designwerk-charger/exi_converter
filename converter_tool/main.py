#!/usr/bin/python3
import datetime
import sys

#sys.path.append("../cmake-build-debug/lib")
sys.path.append("../cmake-build-release/lib")

import exi_converter


print(f"Exi codec version: {exi_converter.__version__}")

codec = exi_converter.ExiCodec()

data = {
  #  "ServiceDiscoveryReq": b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91\xb8',
  #  "PaymentServiceSelectionReq": b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x912\x00\x12\x80',
  #  "AuthorizationReq": b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x90\x08',
  #  "ChargeParameterDiscoveryReq": b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x90\x94\xc8\x00\x08\x00\xf0\x000\x80\xfa\x01\x02\n\x18\x07\xc0\x82\x01@\x080l\x1b\x00\x83\x07\x81p-\x05\x00\x00',
  #  "CableCheckReq": b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x901\x00\x1e\x00',
  #  "PreChargeReq": b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91q\x00\x1e\x01\x88\x12\x00`a\x80\x08\x00',
    "CurrentDemandReq" : b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x90\xd1\x00\x1e\x01\x86\x00!\x00a\x01\xf4\x02\x04\x140\x0f\x80\x000\x81\x90\x06\x10(\x01\x88\x12\x00`',
    "PowerDeliveryReq" : b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91P\x00\x02 \x03\xc1@',
  #  "WeldingDetectionReq": b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x92\x11\x00\x1e\x00'
    "SessionStopReq": b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91\xf0\x00'
}

data2 = [
    ("SessionSetupRes", b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91\xe0 1U,\xc4\xc8\xcd\x14\xc4\xc8\xcc\xd0\x18\x14\xd1\xd2\x80\xc0', '{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionSetupRes":{"ResponseCode":"OK_NewSessionEstablished","EVSEID":"UK123E1234","EVSETimeStamp":1652790080}}}}'),
    ("ServiceDiscoveryRes",b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91\xc0\x01 \x04\x04\x10P\xd7\xd1\x10\xd7\xd0\xda\x18\\\x99\xda[\x99\xc0@d\x80','{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"ServiceDiscoveryRes":{"ResponseCode":"OK","PaymentOptionList":{"PaymentOption":["ExternalPayment"]},"ChargeService":{"ServiceID":1,"ServiceName":"AC_DC_Charging","ServiceCategory":"EVCharging","FreeService":false,"SupportedEnergyTransferMode":{"EnergyTransferMode":["DC_extended"]}}}}}}'),
    ("PaymentServiceSelectionRes" ,b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91@\x00','{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PaymentServiceSelectionRes":{"ResponseCode":"OK"}}}}'),
    ("AuthorizationRes",b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x90\x10\x00\x00','{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"AuthorizationRes":{"ResponseCode":"OK","EVSEProcessing":"Finished"}}}}'),
    ('ChargeParameterDiscoveryRes', b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x8a\x89Z\x1d\x1d\x1c\x0e\x8b\xcb\xdd\xdd\xdd\xcb\x9d\xcc\xcb\x9b\xdc\x99\xcb\xd5\x14\x8b\xd8\xd8[\x9b\xdb\x9aX\xd8[\x0bY^\x1aK\xd0\xd5\xa1\xd1\xd1\xc0\xe8\xbc\xbd\xdd\xdd\xdc\xb9\xdc\xcc\xb9\xbd\xc9\x9c\xbc\xc8\xc0\xc0\xc4\xbc\xc0\xd0\xbd\xe1\xb5\xb1\x91\xcd\xa5\x9c\xb5\xb5\xbd\xc9\x94\x8d\x95\x8d\x91\xcd\x84\xb5\xcd\xa1\x84\xc8\xd4\xd9\x101\x1bK!\x88\x12\xb4::8\x1d\x17\x97\xbb\xbb\xbb\x97;\x99\x977\xb93\x97\xaa)\x17\xb1\xb0\xb77\xb74\xb1\xb0\xb6\x16\xb2\xbc4\x97\xa4)http://www.w3.org/2001/04/xmlenc#sha256B\x01N=\x87\xae$$\xb6\xc3\x05\xfd_\x18\xa2O\xe2\xf2\xf6\x87\xcde\x8dY\xbb\xe5\x1a|\xe3\xf5\xcf\x08o\x01(\xc6\x08\x80D\n\xd4P\x13\xe7\xaaGD\xa6\xe6E<\xad\x9e\xb6\xa9n\xd1\xb0~D(k\xce\x1a\xe4\xce\x99\xf8\xa8\xe5\x13 D\x03\x8f!\x9b\xbe\xd1\xe0k\x94\x02J\x17\x98\xbe\xd8S\xff\x19\x1bldp\'\xf0\x19\xdbuD\x17.[\x01h(\x00\x00\x00\x00\x00\t\x01\xc01C\xe1T \n\xd2\xc8b\x04\x90\x10\x00\x04\x00\x90"C\x82\x1c8\x00E(d\x00\x10 \x81\x80  \xa1\xcc\x02\x08 \x02\x02\x06\x00@\x82\x00 \x900\x04\x10','{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E","Signature":{"SignedInfo":{"CanonicalizationMethod":{"Algorithm":"http://www.w3.org/TR/canonical-exi/"},"SignatureMethod":{"Algorithm":"http://www.w3.org/2001/04/xmldsig-more#ecdsa-sha256"},"Reference":[{"URI":"#id1","Transforms":{"Transform":[{"Algorithm":"http://www.w3.org/TR/canonical-exi/"}]},"DigestMethod":{"Algorithm":"http://www.w3.org/2001/04/xmlenc#sha256"},"DigestValue":"FOPYeuJCS2wwX9XxiiT+Ly9ofNZY1Zu+UafOP1zwhvA="}]},"SignatureValue":{"value":"MEQCIFaigJ89UjolNzIp5Wz1tUt2jYPyIUNecNcmdM/FRyiZAiAceQzd9o8DXKASULzF9sKf+MjbYyOBP4DO26oguXLYCw=="}}},"Body":{"ChargeParameterDiscoveryRes":{"ResponseCode":"OK","EVSEProcessing":"Finished","SAScheduleList":{"SAScheduleTuple":[{"SAScheduleTupleID":1,"PMaxSchedule":{"PMaxScheduleEntry":[{"RelativeTimeInterval":{"start":0,"duration":3600},"PMax":{"Multiplier":0,"Unit":"W","Value":11000}}]},"SalesTariff":{"Id":"id1","SalesTariffID":10,"NumEPriceLevels":2,"SalesTariffEntry":[{"RelativeTimeInterval":{"start":0},"EPriceLevel":1},{"RelativeTimeInterval":{"start":1801,"duration":1799},"EPriceLevel":2}]}}]},"DC_EVSEChargeParameter":{"DC_EVSEStatus":{"NotificationMaxDelay":100,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEMaximumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":4},"EVSEMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":230},"EVSEMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":4},"EVSEMinimumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":2},"EVSEMinimumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":4},"EVSEPeakCurrentRipple":{"Multiplier":1,"Unit":"A","Value":4}}}}}}'),
    ("CableCheckRes", b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x90@\x00\x00\x00 @\x00', '{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"CableCheckRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEProcessing":"Finished"}}}}'),
    ("PreChargeRes", b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91\x80\x00\x00\x00 @\xc4\x0e`\x10\x00', '{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PreChargeRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230}}}}}'),
    ("PowerDeliveryRes", b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91`\x04\x00\x00\x10 ', '{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PowerDeliveryRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"}}}}}'),
    ("CurrentDemandRes", b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x90\xe0\x00\x00\x00 @\xc4\x0e`\x100\xc0\x04" 1\x03`\x10\x06\x18V\x01\x01\x05\x0e\x80p\x0cUK123E1234\x00\x04\x00', '{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"CurrentDemandRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230},"EVSEPresentCurrent":{"Multiplier":0,"Unit":"A","Value":1},"EVSECurrentLimitAchieved":true,"EVSEVoltageLimitAchieved":true,"EVSEPowerLimitAchieved":true,"EVSEMaximumVoltageLimit":{"Multiplier":0,"Unit":"V","Value":600},"EVSEMaximumCurrentLimit":{"Multiplier":0,"Unit":"A","Value":300},"EVSEMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":1000},"EVSEID":"UK123E1234","SAScheduleTupleID":1,"ReceiptRequired":false}}}}'),
    ("PowerDeliveryRes", b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x91`\x04\x00\x00\x10 ', '{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"PowerDeliveryRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"}}}}}'),
    ("WeldingDetectionRes", b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x92 \x00\x00\x00 @\xc4\x0e`\x10\x00', '{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"WeldingDetectionRes":{"ResponseCode":"OK","DC_EVSEStatus":{"NotificationMaxDelay":0,"EVSENotification":"None","EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230}}}}}'),
    ("SessionStopRes", b'\x80\x98\x02\r\xf6\xa6=\xcd\x82$\x0f\x92\x00\x00', '{"V2G_Message":{"Header":{"SessionID":"37DA98F73608903E"},"Body":{"SessionStopRes":{"ResponseCode":"OK"}}}}'),
]

timedeltas = []

for key, value in data.items():
    print(f"Decoding '{key}'")
    tstart = datetime.datetime.now()
    dec = codec.decode(value, len(value), "ns")
    timedeltas.append((datetime.datetime.now()-tstart).total_seconds()*1000)
    print(dec)

print(timedeltas)

print(codec.encode("bla", "ns"))
