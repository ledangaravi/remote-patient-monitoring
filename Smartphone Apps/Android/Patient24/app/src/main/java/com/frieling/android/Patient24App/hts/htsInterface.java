package com.frieling.android.Patient24App.hts;

import android.bluetooth.BluetoothDevice;
import android.content.IntentFilter;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.Calendar;

import com.frieling.android.Patient24App.R;

public interface htsInterface {

     void onTemperatureMeasurementReceived(@NonNull final BluetoothDevice device,
                                           final float temperature, final int unit,
                                           @Nullable final Calendar calendar,
                                           @Nullable final Integer type);

     void onCreate();

     void onDestroy();

     void onDeviceDisconnected(final BluetoothDevice device);
}