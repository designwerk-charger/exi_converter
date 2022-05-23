
ISO15118_2_MSGS_SPECIAL = ["PaymentDetails", "MeteringReceipt", "ChargingStatus", "CertificateInstallation",
                           "CertificateUpdate", "ServiceDetail"]
ISO15118_2_MSGS = ["SessionSetup", "ServiceDiscovery", "PaymentServiceSelection", "Authorization",
                   "ChargeParameterDiscovery", "CableCheck", "PreCharge", "PowerDelivery", "CurrentDemand",
                   "WeldingDetection", "SessionStop"] + ISO15118_2_MSGS_SPECIAL
ISO15118_2_MAIN_TYPES = ["MessageHeaderType", "BodyType"]
