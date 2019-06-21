package com.frieling.android.Patient24App.newGUI;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProviders;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

import com.frieling.android.Patient24App.R;

import static com.frieling.android.Patient24App.newGUI.sensorData.initSensorDataArray;
import static com.frieling.android.Patient24App.newGUI.sensorData.initSensorDataList;

public class LiveDashboardFragment extends Fragment {

    public List<sensorData> sensorDataList = initSensorDataList();
    private sensorData[] BLEdata = initSensorDataArray(); //TODO make this device agnostic (by changing to arraylist?)


    private LiveDashboardCardAdapter mAdapter;

    protected LiveDataViewModel mViewModel;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(false);


        /** View MODEL stuff
         *
         */
        mViewModel = ViewModelProviders.of(getActivity()).get(LiveDataViewModel.class);


        // Create the observer which updates the UI.
        final Observer<List<sensorData>> nameObserver = new Observer<List<sensorData>>() {
            @Override
            public void onChanged(@Nullable final List<sensorData> newValue) {
                sensorDataList = newValue;
                //TODO update using DiffUtil
                Log.w("logObject","---------------in fragment----------------------");
                for (int i  = 0;  i < newValue.size(); i++){
                    BLEdata[i] = newValue.get(i);
                    newValue.get(i).logObject();
                }
                Log.w("logObject","---------------in adapter----------------------");
                if (mAdapter != null)
                    mAdapter.notifyDataSetChanged();
            }
        };

        // Observe the LiveData, passing in this activity as the LifecycleOwner and the observer.
       mViewModel.getCurrentValue().observe(getActivity(), nameObserver);
    }


    @Override
    public View onCreateView(
            @NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        // Inflate the layout for this fragment with the ProductGrid theme
        View view = inflater.inflate(R.layout.live_dashboard, container, false);



        // Set up the RecyclerView
        RecyclerView recyclerView = view.findViewById(R.id.recycler_view);
        recyclerView.setHasFixedSize(true);
        recyclerView.setLayoutManager(new LinearLayoutManager(getContext()));

        //set up adapter
        mAdapter = new LiveDashboardCardAdapter(BLEdata);
        recyclerView.setAdapter(mAdapter);

        mAdapter.notifyDataSetChanged();

        int largePadding = getResources().getDimensionPixelSize(R.dimen.shr_product_grid_spacing);
        int smallPadding = getResources().getDimensionPixelSize(R.dimen.shr_product_grid_spacing_small);
        recyclerView.addItemDecoration(new LiveDashboardCardItemDecoration(largePadding, smallPadding));

        return view;
    }

}