package com.frieling.android.Patient24App.newGUI.BLE_Callbacks;

import android.bluetooth.BluetoothDevice;
import android.util.Log;

import androidx.annotation.NonNull;

import no.nordicsemi.android.ble.callback.profile.ProfileDataCallback;
import no.nordicsemi.android.ble.data.Data;


public abstract class stepCountDataCallback implements p24CharacteristicCallback, ProfileDataCallback {

    @Override
    public void onDataReceived(@NonNull final BluetoothDevice device, @NonNull final Data data) {
        if (data.size() < 2) {
            Log.w("P24", "in p24datacallback. invalid data received");
            onInvalidDataReceived(device, data);
            return;
        }

        // Read flags
//        int offset = 0;
//        final int flags = data.getIntValue(Data.FORMAT_UINT8, offset);
//        final int hearRateType = (flags & 0x01) == 0 ? Data.FORMAT_UINT8 : Data.FORMAT_UINT16;
//        offset += 1;
//
//        // Validate packet length. The type's lower nibble is its length.
//        if (data.size() < 1 + (hearRateType & 0x0F)) {
//            onInvalidDataReceived(device, data);
//            return;
//        }
        int offset = 0;
        final int formatType = Data.FORMAT_UINT32;
        final int value = data.getIntValue(formatType, offset);
        // offset += hearRateType & 0xF;

        // ...
        Log.w("P24", "in p24datacallback");

        // Report the parsed value(s)
        onStepCountReceived(value);
    }
}

