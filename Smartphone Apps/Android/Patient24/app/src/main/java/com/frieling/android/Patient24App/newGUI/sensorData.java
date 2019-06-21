package com.frieling.android.Patient24App.newGUI;

import android.text.SpannableString;
import android.text.style.RelativeSizeSpan;
import android.util.Log;

import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;

import com.frieling.android.Patient24App.R;

class sensorData {

    String sensorReading;
    int BPsys = 120;
    int BPdia = 80;
    String sensorName;
    String units;
    int imageId;

    sensorData(String name, String value){
        this.sensorReading = value;
        this.sensorName = name;
        this.imageId = getDrawableId();
        this.units = getUnits();
    }

    public static sensorData[]  initSensorDataArray(){
        sensorData[] BLEdata = new sensorData[5];

        BLEdata[0] = new sensorData("Heart Rate", "-");
        BLEdata[1] = new sensorData("Skin \nTemp", "-");
        BLEdata[2] = new sensorData("Blood Oxygen", "-");
        BLEdata[3] = new sensorData("Step Count", "-");
        BLEdata[4] = new sensorData("Battery Level", "-");

        return BLEdata;
    }

    public static List<sensorData> initSensorDataList(){
        List<sensorData> sensorDataList = new ArrayList<>();
        sensorData data0 = new sensorData("Heart Rate", "-");
        sensorData data1 = new sensorData("Skin \nTemp", "-");
        sensorData data2 = new sensorData("Blood Oxygen", "-");
        sensorData data3 = new sensorData("Step Count", "-");
        sensorData data4 = new sensorData("Battery Level", "-");

        sensorDataList.add(data0);
        sensorDataList.add(data1);
        sensorDataList.add(data2);
        sensorDataList.add(data3);
        sensorDataList.add(data4);

        return sensorDataList;
    }

    private int getDrawableId() {
        String resourceName = "0";
        try {
            switch (sensorName){
                case "Skin \nTemp": resourceName = "icon_temperature"; break;
                case "Battery Level": resourceName = "icon_battery_level"; break;
                case "Heart Rate": resourceName = "icon_heart_rate"; break;
                case "Step Count": resourceName = "icon_pedometer"; break;
                case "Blood Oxygen": resourceName = "icon_blood_oxygen"; break;
                case "Blood Pressure": resourceName = "icon_blood_pressure"; break;
                default: resourceName = "icon_notfound";
            }
            Field idField = R.drawable.class.getDeclaredField(resourceName);
            return idField.getInt(idField);
        } catch (Exception e) {
            Log.w("jonas", "resource not found: " + resourceName);
            throw new RuntimeException("No resource ID found for: "
                    + resourceName + " / ", e);
        }
    }

    private String getUnits(){
        String units;
        switch (sensorName){
            case "Skin \nTemp": units = "°C"; break;
            case "Battery Level": units = "%"; break;
            case "Heart Rate": units = "bpm"; break;
            case "Step Count": units = "steps"; break;
            case "Blood Oxygen": units = "SpO\u2082"; break;
            case "Blood Pressure": units = "mmHg"; break;
            default: units = "?";
        }
        return units;
    }

    public void logObject(){
        Log.w("logObject", "sensorName: " + sensorName + "          sensorReading: " + sensorReading + "           imageId: " + imageId);
    }


    /**
     * Blood Pressure Stuff
     * */
    public static String makeBPstring(int sys, int dia){

        String str = sys +"\n" + dia;

        return str;
    }

    public static SpannableString makeBPspannable(String string){
        SpannableString BPstring = new SpannableString(string);
        int start = BPstring.length()-3;
        int end = BPstring.length();
        BPstring.setSpan(new RelativeSizeSpan(0.5f),start,end,0);
        BPstring.setSpan(new RelativeSizeSpan(0.5f),3,6,0);

        return BPstring;
    }
}
