package com.frieling.android.Patient24App.newGUI.BLE_Callbacks;

public interface p24CharacteristicCallback {
    void onStepCountReceived(final int stepCount);
    void onBloodOxReceived(final int bloodOxValue);
}
