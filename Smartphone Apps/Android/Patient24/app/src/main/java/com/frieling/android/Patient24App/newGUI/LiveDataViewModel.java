package com.frieling.android.Patient24App.newGUI;


import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.List;



public class LiveDataViewModel extends ViewModel {


        private  MutableLiveData<List<sensorData>> currentValue = new MutableLiveData<List<sensorData>>();

        public MutableLiveData<List<sensorData>> getCurrentValue() {
                if (currentValue == null) {
                        currentValue = new MutableLiveData<List<sensorData>>();
                }
                return currentValue;
        }
}
