#!/usr/bin/python3
import datetime
import sys

#sys.path.append("../cmake-build-debug/lib")
#sys.path.append("../cmake-build-release/lib")
from statistics import mean

import exi_converter


print(f"Exi codec version: {exi_converter.__version__}")

codec = exi_converter.ExiCodec()

din_ns = "urn:din:70121:2012:MsgDef"
din_msgs = [
    ("SessionSetupReq", b'\x80\x9a\x00@\x11\xd0\x18\x03\x813\xbcd,\x00', '{"V2G_Message":{"Header":{"SessionID":"00"},"Body":{"SessionSetupReq":{"EVCCID":"00E04CEF190B"}}}}'),
    ("ServiceDiscoveryReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91\x98', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ServiceDiscoveryReq":{}}}}'),
    ("ServicePaymentSelectionReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91\xb2\x00\x12\x80', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ServicePaymentSelectionReq":{"SelectedPaymentOption":"ExternalPayment","SelectedServiceList":{"SelectedService":[{"ServiceID":1}]}}}}}'),
    ("ContractAuthenticationReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x90\xb8', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ContractAuthenticationReq":{}}}}'),
    ("ChargeParameterDiscoveryReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x90q\x91@\x0f\x00\x00\xc2\x03\xe8\x04H\n\x05\x04\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ChargeParameterDiscoveryReq":{"EVRequestedEnergyTransferType":"DC_extended","DC_EVChargeParameter":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40}}}}}}'),
    ("CableCheckReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x90\x11@\x0f\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"CableCheckReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60}}}}}'),
    ("PreChargeReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91Q@\x0f\x00\xc1B@\x0c\x0c\x0c\x00@', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"PreChargeReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVTargetVoltage":{"Multiplier":0,"Unit":"V","Value":400},"EVTargetCurrent":{"Multiplier":0,"Unit":"A","Value":1}}}}}'),
    ("PowerDeliveryReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x912"\x80\x1e\n\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"PowerDeliveryReq":{"ReadyToChargeState":true,"DC_EVPowerDeliveryParameter":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"ChargingComplete":true}}}}}'),
    ("CurrentDemandReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x90\xd1@\x0f\x00\xc0\xc0\x04\x04\x05\x02\x80\x00\x0c >\x80@\x80\xe1\x80|\x00\x01\x81\x03 \x0c\x08\x14\x00\xc1B@\x0c\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"CurrentDemandReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60},"EVTargetCurrent":{"Multiplier":0,"Unit":"A","Value":1},"EVMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":40},"EVMaximumCurrentLimit":{"Multiplier":-3,"Unit":"A","Value":32000},"EVMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":8000},"BulkChargingComplete":false,"ChargingComplete":false,"RemainingTimeToFullSoC":{"Multiplier":0,"Unit":"s","Value":100},"RemainingTimeToBulkSoC":{"Multiplier":0,"Unit":"s","Value":80},"EVTargetVoltage":{"Multiplier":0,"Unit":"V","Value":400}}}}}'),
    ("WeldingDetectionReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x92\x11@\x0f\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"WeldingDetectionReq":{"DC_EVStatus":{"EVReady":true,"EVErrorCode":"NO_ERROR","EVRESSSOC":60}}}}}'),
    ("SessionStopReq", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91\xf0', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"SessionStopReq":{}}}}'),
    ("SessionSetupRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91\xe0 \x15&\xa2i\x8d\x80\x1f{\xdaR\x80\xc0', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"SessionSetupRes":{"ResponseCode":"OK_NewSessionEstablished","EVSEID":"49A89A6360","DateTimeNow":1653911419}}}}'),
    ("ServiceDiscoveryRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91\xa0\x01 \x02A\x00\xc4', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ServiceDiscoveryRes":{"ResponseCode":"OK","PaymentOptions":{"PaymentOption":["ExternalPayment"]},"ChargeService":{"ServiceTag":{"ServiceID":1,"ServiceCategory":"EVCharging"},"FreeService":false,"EnergyTransferType":"DC_extended"}}}}}'),
    ("ServicePaymentSelectionRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91\xc0\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ServicePaymentSelectionRes":{"ResponseCode":"OK"}}}}'),
    ("ContractAuthenticationRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x90\xc0\x00\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ContractAuthenticationRes":{"ResponseCode":"OK","EVSEProcessing":"Finished"}}}}'),
    ("ChargeParameterDiscoveryRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x90\x80\x00\x00\x04\x00\x00\x00\x04\x80\xe02\x00EP@\x86@\x02\x01\x80 \x10\x1c9\x80A\x01@\x10\x10\x0c\x00\x81\x01@\x10H\x06\x00\x82\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"ChargeParameterDiscoveryRes":{"ResponseCode":"OK","EVSEProcessing":"Finished","SAScheduleList":{"SAScheduleTuple":[{"SAScheduleTupleID":1,"PMaxSchedule":{"PMaxScheduleID":0,"PMaxScheduleEntry":[{"RelativeTimeInterval":{"start":0,"duration":3600},"PMax":200}]}}]},"DC_EVSEChargeParameter":{"DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":100,"EVSENotification":"None"},"EVSEMaximumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":4},"EVSEMaximumPowerLimit":{"Multiplier":1,"Unit":"W","Value":230},"EVSEMaximumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":4},"EVSEMinimumCurrentLimit":{"Multiplier":1,"Unit":"A","Value":2},"EVSEMinimumVoltageLimit":{"Multiplier":1,"Unit":"V","Value":4},"EVSEPeakCurrentRipple":{"Multiplier":1,"Unit":"A","Value":4}}}}}}'),
    ("CableCheckRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x90 \x00@\x80\x00\x00\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"CableCheckRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"},"EVSEProcessing":"Finished"}}}}'),
    ("PreChargeRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91`\x00@\x80\x00\x01\x82\x870\x08\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"PreChargeRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230}}}}}'),
    ("PowerDeliveryRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x91@\x04 @\x00\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"PowerDeliveryRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"}}}}}'),
    ("CurrentDemandRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x90\xe0\x00@\x80\x00\x01\x82\x870\x08\x18\x18\x00\x80D\xc0', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"CurrentDemandRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230},"EVSEPresentCurrent":{"Multiplier":0,"Unit":"A","Value":1},"EVSECurrentLimitAchieved":false,"EVSEVoltageLimitAchieved":true,"EVSEPowerLimitAchieved":true}}}}'),
    ("WeldingDetectionRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x92 \x00@\x80\x00\x01\x82\x870\x08\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"WeldingDetectionRes":{"ResponseCode":"OK","DC_EVSEStatus":{"EVSEIsolationStatus":"Valid","EVSEStatusCode":"EVSE_Ready","NotificationMaxDelay":0,"EVSENotification":"None"},"EVSEPresentVoltage":{"Multiplier":0,"Unit":"V","Value":230}}}}}'),
    ("SessionStopRes", b'\x80\x9a\x02/\xe9;=\x12g\xa2\xc9\x92\x00\x00', '{"V2G_Message":{"Header":{"SessionID":"BFA4ECF4499E8B26"},"Body":{"SessionStopRes":{"ResponseCode":"OK"}}}}'),
]


def decode(msgs, ns):
    timedeltas = []
    for (name, raw, jsons) in msgs:
        # print(f"Decoding '{name}'")
        tstart = datetime.datetime.now()
        dec = codec.decode(raw, len(raw), ns)
        timedeltas.append((datetime.datetime.now()-tstart).total_seconds()*1000)

    td_str = ', '.join([f"{td:.2f}" for td in timedeltas])
    print(f"decoding times: {td_str}")
    print(f"decoding mean: {mean(timedeltas):.3f}ms")


def encode(msgs, ns):
    timedeltas = []
    for (name, raw, jsons) in msgs:
        # print(f"Encoding '{name}'")
        tstart = datetime.datetime.now()
        enc = codec.encode(jsons, ns)
        timedeltas.append((datetime.datetime.now()-tstart).total_seconds()*1000)

    td_str = ', '.join([f"{td:.2f}" for td in timedeltas])
    print(f"encoding times: {td_str}")
    print(f"encoding mean: {mean(timedeltas):.3f}ms")

def bruteforce_enc_dec(msgs, ns, times: int):
    timedeltas = []
    for i in range(times):
        for (name, raw, jsons) in msgs:
            tstart = datetime.datetime.now()
            dec = codec.decode(raw, len(raw), ns)
            enc = codec.encode(dec, ns)
            timedeltas.append((datetime.datetime.now()-tstart).total_seconds()*1000)
            if raw != enc:
                raise RuntimeError(f"failed encoding or decoding {name}")
    print(f"decoding and encoding all messages for {times} times: mean={mean(timedeltas):.3f}ms, max={max(timedeltas):.3f}ms, min={min(timedeltas):.3f}ms")



msgs_str = ", ".join([name for (name, raw, jsons) in din_msgs])
print(f"Decoding and Encoding the following messages: {msgs_str}")
decode(din_msgs, din_ns)
encode(din_msgs, din_ns)

bruteforce_enc_dec(din_msgs, din_ns, 10000)
