package com.frieling.android.Patient24App.newGUI;

import java.util.ArrayList;
import java.util.List;

public class historyAlertData {

    public String alert_category;
    public String alert_Text;
    public String alert_Time;

    public historyAlertData(String alert_category, String alert_Time,String alert_Text) {
        this.alert_category = alert_category;
        this.alert_Text = alert_Text;
        this.alert_Time = alert_Time;
    }

    public static final List<historyAlertData> alertList = new ArrayList<>();


    public static List<historyAlertData> initFakeHistory(){

        //Heart Rate
        alertList.add(
                new historyAlertData(
                        "Heart Rate",
                        "21/05/19 at 13:23",
                        "Heart Rate above 160")
        );

        //Skin Temperature
        alertList.add(
                new historyAlertData(
                        "Skin Temperature",
                        "23/05/19 at 16:52",
                        "Skin Temperature above 35°C")
        );

        //Skin Temperature
        alertList.add(
                new historyAlertData(
                        "Blood Oxygen",
                        "23/05/19 at 16:52",
                        "Blood Oxygen below 96")
        );

        //Aggregate
        alertList.add(
                new historyAlertData(
                        "Sensor Aggregate Anomaly Detected",
                        "29/05/19 at 19:52",
                        "Heart Rate: 134\n" +
                                "Skin Temp: 43\n" +
                                "Blood Oxygen: 98\n" +
                                "Step Count: 43"
                )
        );

        //Step Count
        //Aggregate
        alertList.add(
                new historyAlertData(
                        "Step Count",
                        "29/05/19 at 12:00",
                        "Step Count in the morning below 600"
                )

        );


        return alertList;
    }
}
