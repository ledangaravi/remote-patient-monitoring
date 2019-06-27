package com.frieling.android.Patient24App.newGUI.BLE_Callbacks;

import android.bluetooth.BluetoothDevice;
import android.util.Log;

import androidx.annotation.NonNull;

import no.nordicsemi.android.ble.callback.profile.ProfileDataCallback;
import no.nordicsemi.android.ble.data.Data;

public abstract class bloodOxDataCallback implements p24CharacteristicCallback, ProfileDataCallback {

    @Override
    public void onDataReceived(@NonNull final BluetoothDevice device, @NonNull final Data data) {
        if (data.size() < 2) {
            Log.w("P24", "in p24datacallback. invalid data received");
            onInvalidDataReceived(device, data);
            return;
        }

        int offset = 0;
        final int formatType = Data.FORMAT_UINT8;
        final int value = data.getIntValue(formatType, offset);

        Log.w("P24", "in bloodOxdatacallback");

        // Report the parsed value(s)
        onBloodOxReceived(value);
    }

}
