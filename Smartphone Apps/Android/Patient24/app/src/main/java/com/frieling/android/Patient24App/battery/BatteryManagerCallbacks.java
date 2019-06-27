package com.frieling.android.Patient24App.battery;

import no.nordicsemi.android.ble.BleManagerCallbacks;
import no.nordicsemi.android.ble.common.profile.battery.BatteryLevelCallback;

public interface BatteryManagerCallbacks extends BleManagerCallbacks, BatteryLevelCallback {
}
