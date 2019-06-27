package com.frieling.android.Patient24App.newGUI;

import android.content.Context;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.RecyclerView;

import com.jjoe64.graphview.GraphView;

import com.frieling.android.Patient24App.R;



public class LiveDashboardCardViewHolder extends RecyclerView.ViewHolder {


    Context context;

    public ImageView sensorImage;
    public TextView sensor_Name;
    public TextView sensor_Reading;
    public TextView sensor_units_view;
    public LinearLayout expandedArea;
    public TextView status_indicator;

    public GraphView graph;

    public LiveDashboardCardViewHolder(@NonNull View itemView) {
        super(itemView);
        //TODO: Find and store views from itemView
        sensorImage = itemView.findViewById(R.id.sensorImage_field);
        sensor_Name = itemView.findViewById(R.id.sensorName_field);
        sensor_Reading = itemView.findViewById(R.id.sensorValue_field);
        sensor_units_view = itemView.findViewById(R.id.units_field);

        expandedArea = itemView.findViewById(R.id.expanded_area);

        graph = itemView.findViewById(R.id.graph); //is red because the expanded section is #included

        status_indicator = itemView.findViewById(R.id.health_status_indicator);

        context = itemView.getContext();
    }

    public void setIndicatorColor(sensorData data){

        status_indicator.setVisibility(View.VISIBLE);


        double  value = Double.parseDouble(data.sensorReading);

        String sensorName = data.sensorName;
        if (context!=null) {

            switch (sensorName) {
                case "Heart Rate":
                    if (value > 150 || value < 50)
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_unhealthy_color));
                    else
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_healthy_color));
                    break;
                case "Skin \nTemp":
                    if (value > 34 || value < 20)
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_unhealthy_color));
                    else
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_healthy_color));
                    break;
                case "Blood Oxygen":
                    if (value > 100 || value < 96)
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_unhealthy_color));
                    else
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_healthy_color));
                    break;
                case "Step Count":
                    if (value < 100)
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_unhealthy_color));
                    else
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_healthy_color));
                    break;
                case "Battery Level":
                    if (value < 30)
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_unhealthy_color));
                    else
                        status_indicator.setTextColor(ContextCompat.getColor(context, R.color.status_healthy_color));
                    break;
                //case "Blood Pressure":
//                        sensor_Reading.setTextSize(40f);
                default:
                    break;
            }
        }
       // else
            //status_indicator.setVisibility(View.INVISIBLE);
    }
}
