package com.frieling.android.Patient24App.newGUI;

import android.text.SpannableString;
import android.text.style.RelativeSizeSpan;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.RecyclerView;


import com.jjoe64.graphview.GridLabelRenderer;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import java.util.Date;
import java.util.Random;

import com.frieling.android.Patient24App.R;

/**
 * Adapter used to show a simple grid of products.
 */
public class LiveDashboardCardAdapter extends RecyclerView.Adapter<LiveDashboardCardViewHolder> {

    private int mExpandedPosition = -1;
    private int counter = 0;
    private static final int HEARTRATE = 0;
    private static final int TEMP = 1;
    private static final int BLOODOX = 2;
    private static final int STEPCOUNT = 3;
    private static final int BATTERY = 4;
       private static final int OTHER = 0;
       @Override
       public int getItemViewType(int position) {
           switch (position) {
               case 0:
                   return HEARTRATE;
               case 1:
                   return TEMP;
               case 2:
                   return BLOODOX;
               case 3:
                   return STEPCOUNT;
               case 4:
                   return BATTERY;
           }
           return 0;
       }



    private sensorData[] sensorDataArray;
    LineGraphSeries<DataPoint> series0 = new LineGraphSeries<>();
    LineGraphSeries<DataPoint> series1 = new LineGraphSeries<>();
    LineGraphSeries<DataPoint> series2 = new LineGraphSeries<>();
    LineGraphSeries<DataPoint> series3 = new LineGraphSeries<>();
    LineGraphSeries<DataPoint> series4 = new LineGraphSeries<>();

    LiveDashboardCardAdapter(sensorData[] data) {
        this.sensorDataArray = data;
    }

    @NonNull
    @Override
    public LiveDashboardCardViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {

        View layoutView = null;
        LiveDashboardCardViewHolder holder = null;
        int lineColor = ContextCompat.getColor(parent.getContext(),R.color.graphColor);

        switch (viewType){
            case (HEARTRATE):
                layoutView = LayoutInflater.from(parent.getContext()).inflate(R.layout.live_dashboard_card, parent, false);
                series0.setColor(lineColor);
                series0.setThickness(12);

                holder = new LiveDashboardCardViewHolder(layoutView);
                holder.graph.addSeries(series0);
                break;
            case (TEMP):
                layoutView = LayoutInflater.from(parent.getContext()).inflate(R.layout.live_dashboard_card, parent, false);
                series1.setColor(lineColor);
                series1.setThickness(12);

                holder = new LiveDashboardCardViewHolder(layoutView);
                holder.graph.addSeries(series1);
                break;
            case (BLOODOX):
                layoutView = LayoutInflater.from(parent.getContext()).inflate(R.layout.live_dashboard_card, parent, false);
                series2.setColor(lineColor);
                series2.setThickness(12);

                holder = new LiveDashboardCardViewHolder(layoutView);
                holder.graph.addSeries(series2);
                break;
            case (STEPCOUNT):
                layoutView = LayoutInflater.from(parent.getContext()).inflate(R.layout.live_dashboard_card, parent, false);
                series3.setColor(lineColor);
                series3.setThickness(12);

                holder = new LiveDashboardCardViewHolder(layoutView);
                holder.graph.addSeries(series3);
                break;
            case (BATTERY):
                layoutView = LayoutInflater.from(parent.getContext()).inflate(R.layout.live_dashboard_card, parent, false);
                series4.setColor(lineColor);
                series4.setThickness(12);

                holder = new LiveDashboardCardViewHolder(layoutView);
                holder.graph.addSeries(series4);
                break;
        }


//                layoutView = LayoutInflater.from(parent.getContext()).inflate(R.layout.live_dashboard_card, parent, false);
//                holder = new LiveDashboardCardViewHolder(layoutView);



        holder.graph.getGridLabelRenderer().setHorizontalLabelsVisible(false);
        holder.graph.getGridLabelRenderer().setGridStyle(GridLabelRenderer.GridStyle.HORIZONTAL);
        holder.graph.getGridLabelRenderer().setPadding(40);

        holder.graph.getViewport().setXAxisBoundsManual(true);
        holder.graph.getViewport().setMinX(0);
        holder.graph.getViewport().setMaxX(40);


        return holder;
    }





    //The below code tells our RecyclerView's adapter what to do with each card, using a ViewHolder.
    @Override
    public void onBindViewHolder(@NonNull LiveDashboardCardViewHolder holder, int position) {

//        if(holder.getItemViewType()==HEARTRATE)  {
//            if(sensorDataArray != null && position < sensorDataArray.length) {
//                sensorData reading = sensorDataArray[0];
//                series0.appendData(makeDataPoint(reading.sensorReading), true, 40, false);
//                holder.sensor_Name.setText(reading.sensorName);
//                holder.sensorImage.setImageResource(reading.imageId);
//                holder.sensor_units_view.setText(reading.units);
//            }}
//        else {
            if (sensorDataArray != null && position < sensorDataArray.length) {
                sensorData reading = sensorDataArray[position];
                holder.sensor_Name.setText(reading.sensorName);
                holder.sensorImage.setImageResource(reading.imageId);
                holder.sensor_units_view.setText(reading.units);

                Log.w("test","itemviewtype: " +holder.getItemViewType());

                if (!reading.sensorReading.equals("-")) {
                    reading.logObject();
                    holder.sensor_Reading.setText(reading.sensorReading);

                    holder.setIndicatorColor(reading);


                    if (position == sensorDataArray.length - 1) counter++;


                    switch (holder.getItemViewType()) {
                        case 0:
                            if (reading.sensorName.equals("Heart Rate"))
                              series0.appendData(makeDataPoint(reading.sensorReading), true, 40, false);
                            break;
                        case 1:
                            if (reading.sensorName.equals("Skin \nTemp"))
                                series1.appendData(makeDataPoint(reading.sensorReading), true, 40, true);
                            break;
                        case 2:
                            if (reading.sensorName.equals("Blood Oxygen"))
                                series2.appendData(makeDataPoint(reading.sensorReading), true, 40, true);
                            break;
                        case 3:
                            if (reading.sensorName.equals("Step Count"))
                                series3.appendData(makeDataPoint(reading.sensorReading), true, 40, true);
                            break;
                        case 4:
                            if (reading.sensorName.equals("Battery Level"))
                                series4.appendData(makeDataPoint(reading.sensorReading), true, 40, true);
                            break;
                        default:
                            break;
                    }

                    holder.graph.getViewport().scrollToEnd();
                    Log.w("graph", "position: " + position + "  # of Series on Graph: " + holder.graph.getSeries().size() + "   newReading: " + reading.sensorName);
                }

           // }
        }
        final boolean isExpanded = position==mExpandedPosition;
        holder.expandedArea.setVisibility(isExpanded?View.VISIBLE:View.GONE);
        holder.itemView.setActivated(isExpanded);

        if (isExpanded)
            mExpandedPosition = position;

        holder.itemView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mExpandedPosition = isExpanded ? -1:position;
                notifyItemChanged(mExpandedPosition);
                notifyItemChanged(position);
            }
        });
    }


    @Override
    public int getItemCount() {
        return sensorDataArray.length;
    }


    private DataPoint makeDataPoint(String newReading) {
        Date curDate = new Date();
        Double val = Double.parseDouble(newReading);
        DataPoint newDataPoint = new DataPoint(counter, val);
        return newDataPoint;
    }

















     /* RANDOM DATA GENERATOR
    } *
     * **/
    Random rand = new Random();
    double start = 50;
    private void initializeSensorData(){
        for (int i = 0; i < 10;i++){
            start = start +  0.8*rand.nextInt(10);
            Log.w("init","start: " + start);
            series0.appendData(intDataPoint(start),true, 40, false);
        }
        for (int i = 0; i < 10;i++){
            start = start - 0.5*rand.nextInt(10);
            series0.appendData(intDataPoint(start),true, 40, false);
        }
        for (int i = 0; i < 10;i++){
            start = start + 1*rand.nextInt(10);
            series0.appendData(intDataPoint(start),true, 40, false);
        }
        for (int i = 0; i < 10;i++){
            start = start -  0.4*rand.nextInt(10);
            series0.appendData(intDataPoint(start),true, 40, false);
        }

    }

    private DataPoint intDataPoint(double newReading) {
        Double val = (double) newReading;
        DataPoint newDataPoint = new DataPoint(counter, val);
        return newDataPoint;
    }
}
